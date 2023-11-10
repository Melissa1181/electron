// Copyright (c) 2015 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/common/gin_converters/content_converter.h"

#include <string>

#include "base/containers/fixed_flat_map.h"
#include "content/public/browser/context_menu_params.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/input/native_web_keyboard_event.h"
#include "shell/browser/api/electron_api_web_contents.h"
#include "shell/browser/web_contents_permission_helper.h"
#include "shell/common/gin_converters/blink_converter.h"
#include "shell/common/gin_converters/callback_converter.h"
#include "shell/common/gin_converters/frame_converter.h"
#include "shell/common/gin_converters/gfx_converter.h"
#include "shell/common/gin_converters/gurl_converter.h"
#include "shell/common/gin_helper/dictionary.h"
#include "third_party/blink/public/common/context_menu_data/untrustworthy_context_menu_params.h"
#include "ui/events/keycodes/dom/keycode_converter.h"
#include "ui/events/keycodes/keyboard_code_conversion.h"

namespace {

[[nodiscard]] constexpr base::StringPiece FormControlToInputFieldTypeString(
    const absl::optional<blink::mojom::FormControlType> form_control_type) {
  if (!form_control_type)
    return "none";

  switch (*form_control_type) {
    case blink::mojom::FormControlType::kInputPassword:
      return "password";

    case blink::mojom::FormControlType::kInputText:
      return "plainText";

    // other input types:
    case blink::mojom::FormControlType::kInputButton:
    case blink::mojom::FormControlType::kInputCheckbox:
    case blink::mojom::FormControlType::kInputColor:
    case blink::mojom::FormControlType::kInputDate:
    case blink::mojom::FormControlType::kInputDatetimeLocal:
    case blink::mojom::FormControlType::kInputEmail:
    case blink::mojom::FormControlType::kInputFile:
    case blink::mojom::FormControlType::kInputHidden:
    case blink::mojom::FormControlType::kInputImage:
    case blink::mojom::FormControlType::kInputMonth:
    case blink::mojom::FormControlType::kInputNumber:
    case blink::mojom::FormControlType::kInputRadio:
    case blink::mojom::FormControlType::kInputRange:
    case blink::mojom::FormControlType::kInputReset:
    case blink::mojom::FormControlType::kInputSearch:
    case blink::mojom::FormControlType::kInputSubmit:
    case blink::mojom::FormControlType::kInputTelephone:
    case blink::mojom::FormControlType::kInputTime:
    case blink::mojom::FormControlType::kInputUrl:
    case blink::mojom::FormControlType::kInputWeek:
      return "other";

    // not an input type
    default:
      return "none";
  }
}

}  // namespace

namespace gin {

// static
v8::Local<v8::Value> Converter<ui::MenuSourceType>::ToV8(
    v8::Isolate* isolate,
    const ui::MenuSourceType& in) {
  switch (in) {
    case ui::MENU_SOURCE_MOUSE:
      return StringToV8(isolate, "mouse");
    case ui::MENU_SOURCE_KEYBOARD:
      return StringToV8(isolate, "keyboard");
    case ui::MENU_SOURCE_TOUCH:
      return StringToV8(isolate, "touch");
    case ui::MENU_SOURCE_TOUCH_EDIT_MENU:
      return StringToV8(isolate, "touchMenu");
    case ui::MENU_SOURCE_LONG_PRESS:
      return StringToV8(isolate, "longPress");
    case ui::MENU_SOURCE_LONG_TAP:
      return StringToV8(isolate, "longTap");
    case ui::MENU_SOURCE_TOUCH_HANDLE:
      return StringToV8(isolate, "touchHandle");
    case ui::MENU_SOURCE_STYLUS:
      return StringToV8(isolate, "stylus");
    case ui::MENU_SOURCE_ADJUST_SELECTION:
      return StringToV8(isolate, "adjustSelection");
    case ui::MENU_SOURCE_ADJUST_SELECTION_RESET:
      return StringToV8(isolate, "adjustSelectionReset");
    case ui::MENU_SOURCE_NONE:
      return StringToV8(isolate, "none");
  }
}

// static
bool Converter<ui::MenuSourceType>::FromV8(v8::Isolate* isolate,
                                           v8::Local<v8::Value> val,
                                           ui::MenuSourceType* out) {
  std::string type;
  if (!ConvertFromV8(isolate, val, &type))
    return false;

  if (type == "mouse") {
    *out = ui::MENU_SOURCE_MOUSE;
    return true;
  } else if (type == "keyboard") {
    *out = ui::MENU_SOURCE_KEYBOARD;
    return true;
  } else if (type == "touch") {
    *out = ui::MENU_SOURCE_TOUCH;
    return true;
  } else if (type == "touchMenu") {
    *out = ui::MENU_SOURCE_TOUCH_EDIT_MENU;
    return true;
  } else if (type == "longPress") {
    *out = ui::MENU_SOURCE_LONG_PRESS;
    return true;
  } else if (type == "longTap") {
    *out = ui::MENU_SOURCE_LONG_TAP;
    return true;
  } else if (type == "touchHandle") {
    *out = ui::MENU_SOURCE_TOUCH_HANDLE;
    return true;
  } else if (type == "stylus") {
    *out = ui::MENU_SOURCE_STYLUS;
    return true;
  } else if (type == "adjustSelection") {
    *out = ui::MENU_SOURCE_ADJUST_SELECTION;
    return true;
  } else if (type == "adjustSelectionReset") {
    *out = ui::MENU_SOURCE_ADJUST_SELECTION_RESET;
    return true;
  } else if (type == "none") {
    *out = ui::MENU_SOURCE_NONE;
    return true;
  }

  return false;
}

// static
v8::Local<v8::Value> Converter<blink::mojom::MenuItem::Type>::ToV8(
    v8::Isolate* isolate,
    const blink::mojom::MenuItem::Type& val) {
  switch (val) {
    case blink::mojom::MenuItem::Type::kCheckableOption:
      return StringToV8(isolate, "checkbox");
    case blink::mojom::MenuItem::Type::kGroup:
      return StringToV8(isolate, "radio");
    case blink::mojom::MenuItem::Type::kSeparator:
      return StringToV8(isolate, "separator");
    case blink::mojom::MenuItem::Type::kSubMenu:
      return StringToV8(isolate, "submenu");
    case blink::mojom::MenuItem::Type::kOption:
    default:
      return StringToV8(isolate, "normal");
  }
}

// static
v8::Local<v8::Value> Converter<ContextMenuParamsWithRenderFrameHost>::ToV8(
    v8::Isolate* isolate,
    const ContextMenuParamsWithRenderFrameHost& val) {
  const auto& params = val.first;
  content::RenderFrameHost* render_frame_host = val.second;
  auto dict = gin_helper::Dictionary::CreateEmpty(isolate);
  dict.SetGetter("frame", render_frame_host, v8::DontEnum);
  dict.Set("x", params.x);
  dict.Set("y", params.y);
  dict.Set("linkURL", params.link_url);
  dict.Set("linkText", params.link_text);
  dict.Set("pageURL", params.page_url);
  dict.Set("frameURL", params.frame_url);
  dict.Set("srcURL", params.src_url);
  dict.Set("mediaType", params.media_type);
  dict.Set("mediaFlags", MediaFlagsToV8(isolate, params.media_flags));
  bool has_image_contents =
      (params.media_type == blink::mojom::ContextMenuDataMediaType::kImage) &&
      params.has_image_contents;
  dict.Set("hasImageContents", has_image_contents);
  dict.Set("isEditable", params.is_editable);
  dict.Set("editFlags", EditFlagsToV8(isolate, params.edit_flags));
  dict.Set("selectionText", params.selection_text);
  dict.Set("titleText", params.title_text);
  dict.Set("altText", params.alt_text);
  dict.Set("suggestedFilename", params.suggested_filename);
  dict.Set("misspelledWord", params.misspelled_word);
  dict.Set("selectionRect", params.selection_rect);
#if BUILDFLAG(ENABLE_BUILTIN_SPELLCHECKER)
  dict.Set("dictionarySuggestions", params.dictionary_suggestions);
  dict.Set("spellcheckEnabled", params.spellcheck_enabled);
#else
  dict.Set("spellcheckEnabled", false);
#endif
  dict.Set("frameCharset", params.frame_charset);
  dict.Set("referrerPolicy", params.referrer_policy);
  dict.Set("formControlType", params.form_control_type);

  // NB: inputFieldType is deprecated because the upstream
  // field was removed; we are emulating it now until removal
  dict.Set("inputFieldType",
           FormControlToInputFieldTypeString(params.form_control_type));

  dict.Set("menuSourceType", params.source_type);

  return gin::ConvertToV8(isolate, dict);
}

// static
bool Converter<blink::mojom::PermissionStatus>::FromV8(
    v8::Isolate* isolate,
    v8::Local<v8::Value> val,
    blink::mojom::PermissionStatus* out) {
  bool result;
  if (!ConvertFromV8(isolate, val, &result))
    return false;

  if (result)
    *out = blink::mojom::PermissionStatus::GRANTED;
  else
    *out = blink::mojom::PermissionStatus::DENIED;

  return true;
}

// static
v8::Local<v8::Value> Converter<blink::PermissionType>::ToV8(
    v8::Isolate* isolate,
    const blink::PermissionType& val) {
  using PermissionType = electron::WebContentsPermissionHelper::PermissionType;
  // Based on mappings from content/browser/devtools/protocol/browser_handler.cc
  // Not all permissions are currently used by Electron but this will future
  // proof these conversions.
  switch (val) {
    case blink::PermissionType::ACCESSIBILITY_EVENTS:
      return StringToV8(isolate, "accessibility-events");
    case blink::PermissionType::AR:
      return StringToV8(isolate, "ar");
    case blink::PermissionType::BACKGROUND_FETCH:
      return StringToV8(isolate, "background-fetch");
    case blink::PermissionType::BACKGROUND_SYNC:
      return StringToV8(isolate, "background-sync");
    case blink::PermissionType::CLIPBOARD_READ_WRITE:
      return StringToV8(isolate, "clipboard-read");
    case blink::PermissionType::CLIPBOARD_SANITIZED_WRITE:
      return StringToV8(isolate, "clipboard-sanitized-write");
    case blink::PermissionType::LOCAL_FONTS:
      return StringToV8(isolate, "local-fonts");
    case blink::PermissionType::IDLE_DETECTION:
      return StringToV8(isolate, "idle-detection");
    case blink::PermissionType::MIDI_SYSEX:
      return StringToV8(isolate, "midiSysex");
    case blink::PermissionType::NFC:
      return StringToV8(isolate, "nfc");
    case blink::PermissionType::NOTIFICATIONS:
      return StringToV8(isolate, "notifications");
    case blink::PermissionType::PAYMENT_HANDLER:
      return StringToV8(isolate, "payment-handler");
    case blink::PermissionType::PERIODIC_BACKGROUND_SYNC:
      return StringToV8(isolate, "periodic-background-sync");
    case blink::PermissionType::DURABLE_STORAGE:
      return StringToV8(isolate, "persistent-storage");
    case blink::PermissionType::GEOLOCATION:
      return StringToV8(isolate, "geolocation");
    case blink::PermissionType::CAMERA_PAN_TILT_ZOOM:
    case blink::PermissionType::AUDIO_CAPTURE:
    case blink::PermissionType::VIDEO_CAPTURE:
      return StringToV8(isolate, "media");
    case blink::PermissionType::PROTECTED_MEDIA_IDENTIFIER:
      return StringToV8(isolate, "mediaKeySystem");
    case blink::PermissionType::MIDI:
      return StringToV8(isolate, "midi");
    case blink::PermissionType::WAKE_LOCK_SCREEN:
      return StringToV8(isolate, "screen-wake-lock");
    case blink::PermissionType::SENSORS:
      return StringToV8(isolate, "sensors");
    case blink::PermissionType::STORAGE_ACCESS_GRANT:
      return StringToV8(isolate, "storage-access");
    case blink::PermissionType::VR:
      return StringToV8(isolate, "vr");
    case blink::PermissionType::WAKE_LOCK_SYSTEM:
      return StringToV8(isolate, "system-wake-lock");
    case blink::PermissionType::WINDOW_MANAGEMENT:
      return StringToV8(isolate, "window-management");
    case blink::PermissionType::DISPLAY_CAPTURE:
      return StringToV8(isolate, "display-capture");
    case blink::PermissionType::TOP_LEVEL_STORAGE_ACCESS:
      return StringToV8(isolate, "top-level-storage-access");
    case blink::PermissionType::NUM:
      break;
  }

  switch (static_cast<PermissionType>(val)) {
    case PermissionType::POINTER_LOCK:
      return StringToV8(isolate, "pointerLock");
    case PermissionType::KEYBOARD_LOCK:
      return StringToV8(isolate, "keyboardLock");
    case PermissionType::FULLSCREEN:
      return StringToV8(isolate, "fullscreen");
    case PermissionType::OPEN_EXTERNAL:
      return StringToV8(isolate, "openExternal");
    case PermissionType::SERIAL:
      return StringToV8(isolate, "serial");
    case PermissionType::HID:
      return StringToV8(isolate, "hid");
    case PermissionType::USB:
      return StringToV8(isolate, "usb");
    default:
      return StringToV8(isolate, "unknown");
  }
}

// static
bool Converter<content::StopFindAction>::FromV8(v8::Isolate* isolate,
                                                v8::Local<v8::Value> val,
                                                content::StopFindAction* out) {
  using Val = content::StopFindAction;
  static constexpr auto Lookup =
      base::MakeFixedFlatMapSorted<base::StringPiece, Val>({
          {"activateSelection", Val::STOP_FIND_ACTION_ACTIVATE_SELECTION},
          {"clearSelection", Val::STOP_FIND_ACTION_CLEAR_SELECTION},
          {"keepSelection", Val::STOP_FIND_ACTION_KEEP_SELECTION},
      });
  return FromV8WithLookup(isolate, val, Lookup, out);
}

// static
v8::Local<v8::Value> Converter<content::WebContents*>::ToV8(
    v8::Isolate* isolate,
    content::WebContents* val) {
  if (!val)
    return v8::Null(isolate);
  return electron::api::WebContents::FromOrCreate(isolate, val).ToV8();
}

// static
bool Converter<content::WebContents*>::FromV8(v8::Isolate* isolate,
                                              v8::Local<v8::Value> val,
                                              content::WebContents** out) {
  if (!val->IsObject())
    return false;
  // gin's unwrapping converter doesn't expect the pointer inside to ever be
  // nullptr, so we check here first before attempting to unwrap.
  if (gin_helper::Destroyable::IsDestroyed(val.As<v8::Object>()))
    return false;
  electron::api::WebContents* web_contents = nullptr;
  if (!gin::ConvertFromV8(isolate, val, &web_contents) || !web_contents)
    return false;

  *out = web_contents->web_contents();
  return true;
}

// static
v8::Local<v8::Value> Converter<content::Referrer>::ToV8(
    v8::Isolate* isolate,
    const content::Referrer& val) {
  auto dict = gin_helper::Dictionary::CreateEmpty(isolate);
  dict.Set("url", ConvertToV8(isolate, val.url));
  dict.Set("policy", ConvertToV8(isolate, val.policy));
  return gin::ConvertToV8(isolate, dict);
}

// static
bool Converter<content::Referrer>::FromV8(v8::Isolate* isolate,
                                          v8::Local<v8::Value> val,
                                          content::Referrer* out) {
  gin_helper::Dictionary dict;
  if (!ConvertFromV8(isolate, val, &dict))
    return false;

  if (!dict.Get("url", &out->url))
    return false;

  if (!dict.Get("policy", &out->policy))
    return false;

  return true;
}

// static
bool Converter<content::NativeWebKeyboardEvent>::FromV8(
    v8::Isolate* isolate,
    v8::Local<v8::Value> val,
    content::NativeWebKeyboardEvent* out) {
  gin_helper::Dictionary dict;
  if (!ConvertFromV8(isolate, val, &dict))
    return false;
  if (!ConvertFromV8(isolate, val, static_cast<blink::WebKeyboardEvent*>(out)))
    return false;
  dict.Get("skipIfUnhandled", &out->skip_if_unhandled);
  return true;
}

// static
v8::Local<v8::Value> Converter<content::NativeWebKeyboardEvent>::ToV8(
    v8::Isolate* isolate,
    const content::NativeWebKeyboardEvent& in) {
  return ConvertToV8(isolate, static_cast<blink::WebKeyboardEvent>(in));
}

}  // namespace gin
