#include "includes/hooks.h"
#include "libs/KittyMemory/KittyScanner.h"

namespace hooks {
	swapbuffers::orig o_swapbuffers = nullptr;
	input::inject_event::orig o_inject_event = nullptr;

	void* init(void*) {
		LOGI("2");
		const char* lib_EGL = "libEGL.so";

		do {
			sleep(1);
		} while (!utils::is_library_loaded(lib_EGL));

		void* swap_buffers_addr = DobbySymbolResolver(lib_EGL, "eglSwapBuffers");
		if (swap_buffers_addr == NULL)
			return NULL;

		if (DobbyHook(reinterpret_cast<void*>(swap_buffers_addr), reinterpret_cast<void*>(swapbuffers::hook),
					  reinterpret_cast<void**>(&o_swapbuffers)) != 0)
			return NULL;

		const char* lib_unity = KittyMemory::getMapsByName("libunity.so");

		do {
			sleep(1);
		} while (!utils::is_library_loaded(lib_unity));

		auto maps_unity = KittyMemory::getMapsByName("libunity.so");

		//refer to "Tutorials/Get nativeInject Event Signature.md" to understand this
		auto inject_event = KittyScanner::findRegisterNativeFn(maps_unity, "nativeInjectEvent")
		if(!inject_event.isValid())
			return NULL;

		if (DobbyHook(reinterpret_cast<void*>(inject_event.fnPtr), reinterpret_cast<void*>(input::inject_event::hook),
					  reinterpret_cast<void**>(&o_inject_event)) != 0)
			return NULL;

		LOGI("hooks initialized");
		return NULL;
	}

	EGLBoolean swapbuffers::hook(EGLDisplay dpy, EGLSurface surf) {
		EGLint w, h;
		eglQuerySurface(dpy, surf, EGL_WIDTH, &w);
		eglQuerySurface(dpy, surf, EGL_HEIGHT, &h);

		menu::setup_menu();
		menu::draw_menu(w, h);
		
		return o_swapbuffers(dpy, surf);
	}

    namespace input {
    namespace inject_event {
        jmethodID MotionEvent_getX, MotionEvent_getY, MotionEvent_getAction, MotionEvent_getToolType, MotionEvent_getButtonState, MotionEvent_getAxisValue;
        jmethodID KeyEvent_getUnicodeChar, KeyEvent_getMetaState, KeyEvent_getAction, KeyEvent_getKeyCode, KeyEvent_getScanCode;
        int hook(JNIEnv *env, jobject __this, jobject input_event) {

            jclass motion_event = env->FindClass(("android/view/MotionEvent"));

            if(!motion_event){
                LOGI("Can't find MotionEvent!");
                return o_inject_event(env, __this, input_event);
            }

            ImGuiIO &io = ImGui::GetIO();

            if (env->IsInstanceOf(input_event, motion_event)) {
                if (!MotionEvent_getX) MotionEvent_getX = env->GetMethodID(motion_event, ("getX"), ("(I)F"));
                if (!MotionEvent_getY) MotionEvent_getY = env->GetMethodID(motion_event, ("getY"), ("(I)F"));
                if (!MotionEvent_getToolType) MotionEvent_getToolType = env->GetMethodID(motion_event, ("getToolType"), ("(I)I"));
                if (!MotionEvent_getAction) MotionEvent_getAction = env->GetMethodID(motion_event, ("getAction"), ("()I"));
                if (!MotionEvent_getButtonState) MotionEvent_getButtonState = env->GetMethodID(motion_event, ("getButtonState"), ("()I"));
                if (!MotionEvent_getAxisValue) MotionEvent_getAxisValue = env->GetMethodID(motion_event, ("getAxisValue"), ("(II)F"));

                auto action = env->CallIntMethod(input_event, MotionEvent_getAction);
                int32_t pointer_index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                action &= AMOTION_EVENT_ACTION_MASK;

                switch (action) {
                case AMOTION_EVENT_ACTION_DOWN:
                case AMOTION_EVENT_ACTION_UP:
                    if ((env->CallIntMethod(input_event, MotionEvent_getToolType, pointer_index) == AMOTION_EVENT_TOOL_TYPE_FINGER) ||
                        (env->CallIntMethod(input_event, MotionEvent_getToolType, pointer_index) == AMOTION_EVENT_TOOL_TYPE_UNKNOWN)) {
                        io.AddMousePosEvent(env->CallFloatMethod(input_event, MotionEvent_getX, pointer_index), env->CallFloatMethod(input_event, MotionEvent_getY, pointer_index));
                        io.AddMouseButtonEvent(0, action == AMOTION_EVENT_ACTION_DOWN);
                    }
                    break;
                case AMOTION_EVENT_ACTION_BUTTON_PRESS:
                case AMOTION_EVENT_ACTION_BUTTON_RELEASE: {
                    jint button_state = env->CallIntMethod(input_event, MotionEvent_getButtonState);
                    io.AddMouseButtonEvent(0, (button_state & AMOTION_EVENT_BUTTON_PRIMARY) != 0);
                    io.AddMouseButtonEvent(1, (button_state & AMOTION_EVENT_BUTTON_SECONDARY) != 0);
                    io.AddMouseButtonEvent(2, (button_state & AMOTION_EVENT_BUTTON_TERTIARY) != 0);
                } break;
                case AMOTION_EVENT_ACTION_HOVER_MOVE:
                case AMOTION_EVENT_ACTION_MOVE:
                    io.AddMousePosEvent(env->CallFloatMethod(input_event, MotionEvent_getX, pointer_index), env->CallFloatMethod(input_event, MotionEvent_getY, pointer_index));
                    break;
                case AMOTION_EVENT_ACTION_SCROLL:
                    io.AddMouseWheelEvent(env->CallFloatMethod(input_event, MotionEvent_getAxisValue, AMOTION_EVENT_AXIS_HSCROLL, pointer_index),
                                            env->CallFloatMethod(input_event, MotionEvent_getAxisValue, AMOTION_EVENT_AXIS_VSCROLL, pointer_index));
                    break;
                default:
                    break;
                }

                if (io.WantCaptureMouse) return true;
            }

            jclass key_event = env->FindClass("android/view/KeyEvent");
            if(!key_event){
                LOGI("Can't find KeyEvent!");
                return o_inject_event(env, __this, input_event);
            }

            if (env->IsInstanceOf(input_event, key_event)) {

                if (!KeyEvent_getAction) KeyEvent_getAction = env->GetMethodID(key_event, ("getAction"), ("()I"));
                if (!KeyEvent_getKeyCode) KeyEvent_getKeyCode = env->GetMethodID(key_event, ("getKeyCode"), ("()I"));
                if (!KeyEvent_getUnicodeChar) KeyEvent_getUnicodeChar = env->GetMethodID(key_event, ("getUnicodeChar"), ("(I)I"));
                if (!KeyEvent_getMetaState) KeyEvent_getMetaState = env->GetMethodID(key_event, ("getMetaState"), ("()I"));
                if (!KeyEvent_getScanCode) KeyEvent_getScanCode = env->GetMethodID(key_event, ("getScanCode"), ("()I"));

                jint Action = env->CallIntMethod(input_event, KeyEvent_getAction);
                jint KeyCode = env->CallIntMethod(input_event, KeyEvent_getKeyCode);
                jint MetaState = env->CallIntMethod(input_event, KeyEvent_getMetaState);
                jint ScanCode = env->CallIntMethod(input_event, KeyEvent_getScanCode);

                io.AddKeyEvent(ImGuiMod_Ctrl, (MetaState & AMETA_CTRL_ON) != 0);
                io.AddKeyEvent(ImGuiMod_Shift, (MetaState & AMETA_SHIFT_ON) != 0);
                io.AddKeyEvent(ImGuiMod_Alt, (MetaState & AMETA_ALT_ON) != 0);
                io.AddKeyEvent(ImGuiMod_Super, (MetaState & AMETA_META_ON) != 0);

                switch (Action) {
                case AKEY_EVENT_ACTION_DOWN:
                case AKEY_EVENT_ACTION_UP: {
                    ImGuiKey key = utils::KeyCodeToImGuiKey(KeyCode);
                    if (key != ImGuiKey_None && (Action == AKEY_EVENT_ACTION_DOWN || Action == AKEY_EVENT_ACTION_UP)) {
                        io.AddKeyEvent(key, Action == AKEY_EVENT_ACTION_DOWN);
                        io.SetKeyEventNativeData(key, KeyCode, ScanCode);
                    }
                    if (io.WantCaptureKeyboard && Action != AKEY_EVENT_ACTION_UP) {
                        io.AddInputCharacter(env->CallIntMethod(input_event, KeyEvent_getUnicodeChar, MetaState));
                    }
                    break;
                }
                default:
                    break;
                }
            }
            return o_inject_event(env, __this, input_event);
        }
    };
    };
}
