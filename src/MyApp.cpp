#include "MyApp.h"
#include <AppCore/JSHelpers.h>
#include <JavaScriptCore/JSRetainPtr.h>
#include "pm3-reader/file_func.h"
#include <string>

using namespace std;

#define WINDOW_WIDTH  850
#define WINDOW_HEIGHT 700

MyApp::MyApp() {

  app_ = App::Create();

  window_ = Window::Create(app_->main_monitor(), WINDOW_WIDTH, WINDOW_HEIGHT,
    false, kWindowFlags_Titled | kWindowFlags_Resizable);

  overlay_ = Overlay::Create(window_, 1, 1, 0, 0);

  OnResize(window_.get(), window_->width(), window_->height());

  overlay_->view()->LoadURL("file:///app.html");

  app_->set_listener(this);

  window_->set_listener(this);

  overlay_->view()->set_load_listener(this);

  overlay_->view()->set_view_listener(this);
}

MyApp::~MyApp() {
}

void MyApp::Run() {
  app_->Run();
}

void MyApp::OnUpdate() {
  ///
  /// This is called repeatedly from the application's update loop.
  ///
  /// You should update any app logic here.
  ///
}

void MyApp::OnClose(ultralight::Window* window) {
  app_->Quit();
}

void MyApp::OnResize(ultralight::Window* window, uint32_t width, uint32_t height) {
  ///
  /// This is called whenever the window changes size (values in pixels).
  ///
  /// We resize our overlay here to take up the entire window.
  ///
  overlay_->Resize(width, height);
}

void MyApp::OnFinishLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url) 
{
  ///
  /// This is called when a frame finishes loading on the page.
  ///
}


// JSValueRef => std::string
string JSStringToCppString(JSContextRef ctx, const JSValueRef value) {
  JSStringRef JSString = JSValueToStringCopy(ctx, value, 0);
  size_t stringBufferSize = JSStringGetMaximumUTF8CStringSize(JSString);
  char* buffer = new char[stringBufferSize];
  JSStringGetUTF8CString(JSString, buffer, stringBufferSize);

  string str(buffer);
  delete[] buffer;

  return str;
}


// Получение размера файла
JSValueRef getFileSize(JSContextRef ctx, JSObjectRef function,
  JSObjectRef thisObject, size_t argumentCount, 
  const JSValueRef arguments[], JSValueRef* exception) {

  string filepath = JSStringToCppString(ctx, arguments[0]);
  int fsize = getFileSize(filepath);

  return JSValueMakeNumber(ctx, fsize);
}


void setInputValue(JSContextRef ctx, const string& name, const string& value) {
  JSRetainPtr<JSStringRef> str = adopt(JSStringCreateWithUTF8CString("setInputValue"));
  JSValueRef setInputValueFunc = JSEvaluateScript(ctx, str.get(), 0, 0, 0, 0);

  JSObjectRef funcObj = JSValueToObject(ctx, setInputValueFunc, 0);
    const JSValueRef args[] = {
      JSValueMakeString(ctx, adopt(JSStringCreateWithUTF8CString(name.c_str())).get()),
      JSValueMakeString(ctx, adopt(JSStringCreateWithUTF8CString(value.c_str())).get())
    };
    size_t num_args = sizeof(args) / sizeof(JSValueRef*);
    JSValueRef result = JSObjectCallAsFunction(ctx, funcObj, 0, num_args, args, 0);
}


// Заполнение полей input
JSValueRef setInputsValueCpp(JSContextRef ctx, JSObjectRef function,
  JSObjectRef thisObject, size_t argumentCount, 
  const JSValueRef arguments[], JSValueRef* exception) {

  string filepath = JSStringToCppString(ctx, arguments[0]);
  pm3File slicedFile(filepath);

  /// Слои основания
  setInputValue(ctx, "flayer", slicedFile.getStringOfValueFrom<float>("0", 88, 4, false));
  setInputValue(ctx, "fexpo", slicedFile.getStringOfValueFrom<float>("0", 84, 4, false));
  setInputValue(ctx, "ftranslayer", slicedFile.getStringOfValueFrom<int>("0", 140, 4, false));
  // f lift dist
  setInputValue(ctx, "fliftdist1", slicedFile.getStringOfValueFrom<float>("EXTRA", 16, 4, false));
  setInputValue(ctx, "fliftdist2", slicedFile.getStringOfValueFrom<float>("EXTRA", 28, 4, false));
  // f lift speed
  setInputValue(ctx, "fliftspeed1", slicedFile.getStringOfValueFrom<float>("EXTRA", 20, 4, false));
  setInputValue(ctx, "fliftspeed2", slicedFile.getStringOfValueFrom<float>("EXTRA", 32, 4, false));
  // f retract speed
  setInputValue(ctx, "fretractspeed1", slicedFile.getStringOfValueFrom<float>("EXTRA", 36, 4, false));
  setInputValue(ctx, "fretractspeed2", slicedFile.getStringOfValueFrom<float>("EXTRA", 24, 4, false));

  /// Основные слои
  setInputValue(ctx, "thick", slicedFile.getStringOfValueFrom<float>("HEADER", 15, 4, false));
  setInputValue(ctx, "delay", slicedFile.getStringOfValueFrom<float>("0", 80, 4, false));
  setInputValue(ctx, "expo", slicedFile.getStringOfValueFrom<float>("0", 76, 4, false));
  // lift dist
  setInputValue(ctx, "liftdist1", slicedFile.getStringOfValueFrom<float>("HEADER", 35, 4, false));
  setInputValue(ctx, "liftdist2", slicedFile.getStringOfValueFrom<float>("EXTRA", 56, 4, false));
  // lift speed
  setInputValue(ctx, "liftspeed1", slicedFile.getStringOfValueFrom<float>("HEADER", 39, 4, false));
  setInputValue(ctx, "liftspeed2", slicedFile.getStringOfValueFrom<float>("EXTRA", 60, 4, false));
  // retract speed
  setInputValue(ctx, "retractspeed1", slicedFile.getStringOfValueFrom<float>("EXTRA", 64, 4, false));
  setInputValue(ctx, "retractspeed2", slicedFile.getStringOfValueFrom<float>("EXTRA", 52, 4, false));

  // filepath
  setInputValue(ctx, "filepath", filepath);
  // PREVIEW save
  slicedFile.savePreview("./assets/img/preview.bmp");
  

  return JSValueMakeBoolean(ctx, true);
}


void MyApp::OnDOMReady(ultralight::View* caller,
                       uint64_t frame_id,
                       bool is_main_frame,
                       const String& url) {
  
  auto scoped_context = caller->LockJSContext();
  JSContextRef ctx = (*scoped_context);
  JSObjectRef globalObj = JSContextGetGlobalObject(ctx);

  // обьявление getFileSize()
  JSStringRef name = JSStringCreateWithUTF8CString("getFileSize");
  JSObjectRef getFileSizeFunc = JSObjectMakeFunctionWithCallback(ctx, name, getFileSize);
  JSObjectSetProperty(ctx, globalObj, name, getFileSizeFunc, 0, 0);

  // обьявление setInputsValueCpp()
  name = JSStringCreateWithUTF8CString("setInputsValueCpp");
  JSObjectRef setInputsValueCppFunc = JSObjectMakeFunctionWithCallback(ctx, name, setInputsValueCpp);
  JSObjectSetProperty(ctx, globalObj, name, setInputsValueCppFunc, 0, 0);
  JSStringRelease(name);
}

void MyApp::OnChangeCursor(ultralight::View* caller, Cursor cursor) 
{
  ///
  /// This is called whenever the page requests to change the cursor.
  ///
  /// We update the main window's cursor here.
  ///
  window_->SetCursor(cursor);
}

void MyApp::OnChangeTitle(ultralight::View* caller, const String& title) 
{
  ///
  /// This is called whenever the page requests to change the title.
  ///
  /// We update the main window's title here.
  ///
  window_->SetTitle(title.utf8().data());
}










