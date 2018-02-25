/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Precompiled.h"
#include "Application.h"
#include <dlfcn.h>
#include <android/sensor.h>

struct android_app *        appState = nullptr;

static bool                 appInitialized = false;
static int                  currentWindowWidth = 0;
static int                  currentWindowHeight = 0;
static bool                 suspended = false;
static bool                 surfaceCreated = false;
static bool                 hasFocus = false;

static ASensorManager *     sensorManager = nullptr;
static const ASensor *      accelerometerSensor = nullptr;
static const ASensor *      gyroscopeSensor = nullptr;
static ASensorEventQueue *  sensorEventQueue = nullptr;

static jmethodID            javaMethod_showAlert = nullptr;

static BE1::RHI::Handle     mainContext = BE1::RHI::NullContext;
static BE1::RHI::Handle     mainRenderTarget = BE1::RHI::NullRenderTarget;

static std::wstring JavaStringToWString(JNIEnv *env, jstring javaString) {
    const jchar *raw = env->GetStringChars(javaString, 0);
    jsize len = env->GetStringLength(javaString);

    std::wstring ret;
    ret.assign(raw, raw + len);

    env->ReleaseStringChars(javaString, raw);

    return ret;
}

static jstring WStringToJavaString(JNIEnv *env, const std::wstring &wstr) {
    jstring ret;
    size_t len = wstr.length();

    if (sizeof(wchar_t) == sizeof(jchar)) {
        ret = env->NewString((jchar *)wstr.c_str(), (jsize)len);
    } else {
        jchar *javaChars = (jchar *)malloc((len + 1) * sizeof(jchar));
        for (int i = 0; i < len; i++) {
            // This discards two bytes in wstr[i], but these should be 0 in UTF-16
            javaChars[i] = (jchar)wstr[i];
        }
        javaChars[len] = 0;

        ret = env->NewString(javaChars, (jsize)len);
        free(javaChars);
    }
    return ret;
}

static void SystemLog(int logLevel, const wchar_t *msg) {
    if (logLevel == DevLog) {
        __android_log_print(ANDROID_LOG_VERBOSE, "libTestRenderer", "%ls", msg);
    } else if (logLevel == WarningLog) {
        __android_log_print(ANDROID_LOG_WARN, "libTestRenderer", "%ls", msg);
    } else if (logLevel == ErrorLog) {
        __android_log_print(ANDROID_LOG_ERROR, "libTestRenderer", "%ls", msg);
    } else {
        __android_log_print(ANDROID_LOG_INFO, "libTestRenderer", "%ls", msg);
    }
}

static void SystemError(int errLevel, const wchar_t *msg) {
    JNIEnv *env = BE1::AndroidJNI::GetJavaEnv();

    jstring javaMsg = ToJavaString(env, msg);
    AndroidJNI::CallVoidMethod(env, appState->activity->clazz, javaMethod_showAlert, javaMsg);

    env->DeleteLocalRef(javaMsg);
}

static void DisplayMainContext(BE1::RHI::Handle context, void *dataPtr) {
    static float t0 = BE1::PlatformTime::Milliseconds() / 1000.0f;
    float t = BE1::PlatformTime::Milliseconds() / 1000.0f - t0;

    ::app.Draw(context, mainRenderTarget, t);
}

static void InitDisplay() {
    if (!appInitialized) {
        appInitialized = true;

        currentWindowWidth = ANativeWindow_getWidth(appState->window);
        currentWindowHeight = ANativeWindow_getHeight(appState->window);

        ::app.Init(appState->window);

        ::app.LoadResources();

        mainRenderTarget = ::app.CreateRenderTarget(mainContext);

        BE1::rhi.SetContextDisplayFunc(mainContext, DisplayMainContext, nullptr, false);
    } else {
        BE1::rhi.ActivateSurface(mainContext);
    }
}

static void FindJavaClassesAndMethods() {
    JNIEnv *env = BE1::AndroidJNI::GetJavaEnv();

    jclass javaClassActivity = env->GetObjectClass(appState->activity->clazz);

    javaMethod_showAlert = BE1::AndroidJNI::FindMethod(env, javaClassActivity, "showAlert", "(Ljava/lang/String;)V", false);

    env->DeleteLocalRef(javaClassActivity);
}

static void InitInstance(struct android_app *appState) {
    ::appState = appState;

    FindJavaClassesAndMethods();

    BE1::Str path = BE1::PlatformFile::ExecutablePath();
    path.AppendPath("../../..");
    BE1::Engine::InitBase(path.c_str(), false, SystemLog, SystemError);

    // Set window format to 8888
    ANativeActivity_setWindowFormat(appState->activity, WINDOW_FORMAT_RGBA_8888);

    appState->userData = &g_engine;
    appState->onAppCmd = HandleCmd; // app command callback
    appState->onInputEvent = HandleInput; // input event callback

#ifdef USE_NDK_PROFILER
    monstartup("libTestRenderer.so");
#endif

    // Prepare to monitor accelerometer
    InitSensors();
}

static void ShutdownInstance() {
    app.FreeResources();

    if (mainContext) {
        BE1::rhi.DeleteRenderTarget(mainRenderTarget);

        BE1::rhi.DestroyContext(mainContext);
    }

    app.Shutdown();

    BE1::Engine::ShutdownBase();
}

/*
 * AcquireASensorManagerInstance(android_app* app)
 *    Workaround ASensorManager_getInstance() deprecation false alarm
 *    for Android-N and before, when compiling with NDK-r15
 */
static ASensorManager *AcquireASensorManagerInstance(android_app *app) {
    typedef ASensorManager *(*PF_GETINSTANCEFORPACKAGE)(const char *name);
    void *androidHandle = dlopen("libandroid.so", RTLD_NOW);
    PF_GETINSTANCEFORPACKAGE getInstanceForPackageFunc = (PF_GETINSTANCEFORPACKAGE)dlsym(androidHandle, "ASensorManager_getInstanceForPackage");
    if (getInstanceForPackageFunc) {
        JNIEnv *env = nullptr;
        app->activity->vm->AttachCurrentThread(&env, nullptr);

        jclass android_content_Context = env->GetObjectClass(app->activity->clazz);
        jmethodID midGetPackageName = env->GetMethodID(android_content_Context, "getPackageName", "()Ljava/lang/String;");
        jstring packageName= (jstring)env->CallObjectMethod(app->activity->clazz, midGetPackageName);

        const char *nativePackageName = env->GetStringUTFChars(packageName, 0);
        ASensorManager *mgr = getInstanceForPackageFunc(nativePackageName);
        env->ReleaseStringUTFChars(packageName, nativePackageName);
        app->activity->vm->DetachCurrentThread();
        if (mgr) {
            dlclose(androidHandle);
            return mgr;
        }
    }

    typedef ASensorManager *(*PF_GETINSTANCE)();
    PF_GETINSTANCE getInstanceFunc = (PF_GETINSTANCE) dlsym(androidHandle, "ASensorManager_getInstance");
    // by all means at this point, ASensorManager_getInstance should be available
    assert(getInstanceFunc);
    dlclose(androidHandle);

    return getInstanceFunc();
}

static void InitSensors() {
    sensorManager = AcquireASensorManagerInstance(appState);
    accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
    gyroscopeSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_GYROSCOPE);
    sensorEventQueue = ASensorManager_createEventQueue(sensorManager, appState->looper, LOOPER_ID_USER, nullptr, nullptr);
}

static void ProcessSensors(int32_t id) {
    // If a sensor has data, process it now.
    if (id == LOOPER_ID_USER) {
        if (accelerometerSensor) {
            ASensorEvent event;
            while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0) {
                /*float maginitude = event.acceleration.x * event.acceleration.x + event.acceleration.y * event.acceleration.y;
                if (maginitude * 4 >= event.acceleration.z * event.acceleration.z) {
                    int32_t orientation = ORIENTATION_REVERSE_LANDSCAPE;
                    float angle = atan2f(-event.acceleration.y, event.acceleration.x);
                    if (angle <= -M_PI_2 - M_PI_4) {
                        orientation = ORIENTATION_REVERSE_LANDSCAPE;
                    } else if (angle <= -M_PI_4) {
                        orientation = ORIENTATION_PORTRAIT;
                    } else if (angle <= M_PI_4) {
                        orientation = ORIENTATION_LANDSCAPE;
                    } else if (angle <= M_PI_2 + M_PI_4) {
                        orientation = ORIENTATION_REVERSE_PORTRAIT;
                    }
                    LOGI( "orientation %f %d", angle, orientation);
                }*/
            }
        }
    }
}

static void ResumeSensors() {
    // When our app gains focus, we start monitoring the accelerometer.
    if (accelerometerSensor) {
        ASensorEventQueue_enableSensor(sensorEventQueue, accelerometerSensor);
        // We'd like to get 60 events per second (in us).
        ASensorEventQueue_setEventRate(sensorEventQueue, accelerometerSensor, (1000L / 60) * 1000);
    }
    if (gyroscopeSensor) {
        ASensorEventQueue_enableSensor(sensorEventQueue, gyroscopeSensor);
        // We'd like to get 60 events per second (in microseconds).
        ASensorEventQueue_setEventRate(sensorEventQueue, gyroscopeSensor, (1000L / 60) * 1000);
    }
}

static void SuspendSensors() {
    // When our app loses focus, we stop monitoring the accelerometer.
    // This is to avoid consuming battery while not being used.
    if (accelerometerSensor) {
        ASensorEventQueue_disableSensor(sensorEventQueue, accelerometerSensor);
    }
    if (gyroscopeSensor) {
        ASensorEventQueue_disableSensor(sensorEventQueue, gyroscopeSensor);
    }
}

static void WindowSizeChanged(int w, int h) {
    currentWindowWidth = w;
    currentWindowHeight = h;

    // TODO
}

// Process the next main command.
static void HandleCmd(struct android_app *appState, int32_t cmd) {
    Engine *eng = (Engine *)appState->userData;

    switch (cmd) {
    case APP_CMD_DESTROY:
        /**
        * Command from main thread: the app's activity is being destroyed,
        * and waiting for the app thread to clean up and exit before proceeding.
        */
        break;
    case APP_CMD_PAUSE:
        /**
         * Command from main thread: the app's activity has been paused.
         */
        suspended = true;
        break;
    case APP_CMD_RESUME:
        /**
         * Command from main thread: the app's activity has been resumed.
         */
        suspended = false;
        break;
    case APP_CMD_INIT_WINDOW:
        /**
		 * Command from main thread: a new ANativeWindow is ready for use.  Upon
		 * receiving this command, android_app->window will contain the new window
		 * surface.
		 */
        if (appState->window) {
            surfaceCreated = true;
            InitDisplay();
        }        
        break;
    case APP_CMD_TERM_WINDOW:
        /**
		 * Command from main thread: the existing ANativeWindow needs to be
		 * terminated.  Upon receiving this command, android_app->window still
		 * contains the existing window; after calling android_app_exec_cmd
		 * it will be set to NULL.
		 */
        BE1::rhi.DeactivateSurface(mainContext);
        surfaceCreated = false;
        break;
    case APP_CMD_LOST_FOCUS:
        /**
         * Command from main thread: the app's activity window has lost
         * input focus.
         */
        SuspendSensors();
        hasFocus = false;
        break;
    case APP_CMD_GAINED_FOCUS:
        /**
         * Command from main thread: the app's activity window has gained
         * input focus.
         */
        ResumeSensors();
        hasFocus = true;
        break;
    case APP_CMD_WINDOW_RESIZED:
        /**
         * Command from main thread: the current ANativeWindow has been resized.
         * Please redraw with its new size.
         */
        break;
    case APP_CMD_CONFIG_CHANGED:
        /**
		 * Command from main thread: the current device configuration has changed.
		 */
        break;
    }
}

// Process the next input event.
static int32_t HandleInput(struct android_app *appState, AInputEvent *event) {
    int32_t type = AInputEvent_getType(event);
    int32_t source = AInputEvent_getSource(event);

    if (type == AINPUT_EVENT_TYPE_MOTION) {
        int32_t action = AMotionEvent_getAction(event);
        uint64_t pointerId;
        size_t pointerIndex;
        size_t pointerCount;
        int x, y;

        if ((source & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK) {
            // TODO: Implement this
        } else {
            switch (action & AMOTION_EVENT_ACTION_MASK) {
            case AMOTION_EVENT_ACTION_DOWN:
                pointerId = (uint64_t)AMotionEvent_getPointerId(event, 0);
                x = (int)AMotionEvent_getX(event, 0);
                y = (int)AMotionEvent_getY(event, 0);
                LOGI("AMOTION_EVENT_ACTION_DOWN: %i %i", x, y);
                break;
            case AMOTION_EVENT_ACTION_UP:
                pointerId = (uint64_t)AMotionEvent_getPointerId(event, 0);
                x = (int)AMotionEvent_getX(event, 0);
                y = (int)AMotionEvent_getY(event, 0);
                LOGI("AMOTION_EVENT_ACTION_UP: %i %i", x, y);
                break;
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                pointerIndex = (size_t)((action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
                pointerId = (uint64_t)AMotionEvent_getPointerId(event, pointerIndex);
                x = (int)AMotionEvent_getX(event, pointerIndex);
                y = (int)AMotionEvent_getY(event, pointerIndex);
                LOGI("AMOTION_EVENT_ACTION_POINTER_DOWN: %i %i", x, y);
                break;
            case AMOTION_EVENT_ACTION_POINTER_UP:
                pointerIndex = (size_t)((action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
                pointerId = (uint64_t)AMotionEvent_getPointerId(event, pointerIndex);
                x = (int)AMotionEvent_getX(event, pointerIndex);
                y = (int)AMotionEvent_getY(event, pointerIndex);
                LOGI("AMOTION_EVENT_ACTION_POINTER_UP: %i %i", x, y);
                break;
            case AMOTION_EVENT_ACTION_MOVE:
                // ACTION_MOVE events are batched, unlike the other events.
                pointerCount = AMotionEvent_getPointerCount(event);
                for (size_t i = 0; i < pointerCount; i++) {
                    pointerId = (uint64_t)AMotionEvent_getPointerId(event, i);
                    x = (int)AMotionEvent_getX(event, i);
                    y = (int)AMotionEvent_getY(event, i);
                    LOGI("AMOTION_EVENT_ACTION_MOVE(%i/%i): %i %i", (int)i, (int)pointerCount, x, y);
                }
                break;
            }
        }
        return 1;
    }
    return 0;
}

extern "C" {

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    return JNI_CURRENT_VERSION;
}

} // extern "C"

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app *appState) {
    InitInstance(appState);

    //int t0 = BE1::PlatformTime::Milliseconds();

    // loop waiting for stuff to do.
    while (1) {
        // Read all pending events.
        int id;
        int events;
        android_poll_source *source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((id = ALooper_pollAll(!suspended ? 0 : -1, nullptr, &events, (void **)&source)) >= 0) {
            // Process this event.
            if (source) {
                source->process(appState, source);
            }

            ProcessSensors(id);

            // Check if we are exiting.
            if (appState->destroyRequested != 0) {
                g_engine.gl_context_->SuspendSurface();
                ShutdownInstance();
                return;
            }
        }

        if (surfaceCreated && !suspended) {
            int w = ANativeWindow_getWidth(appState->window);
            int h = ANativeWindow_getHeight(appState->window);
            if (w != currentWindowWidth || h != currentWindowHeight) {
                WindowSizeChanged(w, h);
            }

            app.RunFrame();

            BE1::rhi.DisplayContext(mainContext);           
        }
    }
}