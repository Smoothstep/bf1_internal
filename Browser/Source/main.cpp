#include <include/cef_request_handler.h>
#include <include/cef_browser.h>
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_render_handler.h>
#include <include/cef_life_span_handler.h>

#include <tlhelp32.h>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <istream>
#include <fstream>
#include <sstream>
#include <istream>
#include <thread>

const std::filesystem::path ConfigDirectory = []()
{
    char Path[1024];
    {
        GetModuleFileNameA(GetModuleHandle(nullptr), Path, sizeof(Path));
    }

    return std::filesystem::path(Path).parent_path().append("configs");
}();

class SaveConfigHandler : public CefV8Handler
{
    IMPLEMENT_REFCOUNTING(SaveConfigHandler);

public:
    bool Execute(
        const CefString& Name,
        CefRefPtr<CefV8Value> Object,
        const CefV8ValueList& Arguments,
        CefRefPtr<CefV8Value>& Retval,
        CefString& Exception) override
    {
        Retval = CefV8Value::CreateBool(false);

        if (Arguments.size() != 2)
        {
            return false;
        }

        if (!Arguments[0]->IsString() ||
            !Arguments[1]->IsString())
        {
            return false;
        }

        auto File = Arguments[0]->GetStringValue();
        auto Json = Arguments[1]->GetStringValue();

        std::ofstream Out(std::filesystem::path(ConfigDirectory).append(File.ToString()));
        {
            Out << Json.ToString();
        }

        Retval = CefV8Value::CreateBool(true);

        return true;
    }
};

class DeleteConfigHandler : public CefV8Handler
{
    IMPLEMENT_REFCOUNTING(DeleteConfigHandler);

public:
    bool Execute(
        const CefString& Name,
        CefRefPtr<CefV8Value> Object,
        const CefV8ValueList& Arguments,
        CefRefPtr<CefV8Value>& Retval,
        CefString& Exception) override
    {
        Retval = CefV8Value::CreateBool(false);

        if (Arguments.size() != 1)
        {
            return false;
        }

        if (!Arguments[0]->IsString())
        {
            return false;
        }

        const auto File = Arguments[0]->GetStringValue();

        if (File.empty() ||
            File.length() > MAX_PATH)
        {
            std::cerr << "File name length invalid: " << File.length();
            return false;
        }

        const auto FullPath = std::filesystem::path(ConfigDirectory).append(File.ToString()).native();

        Retval = CefV8Value::CreateBool(DeleteFileW(FullPath.c_str()));

        return true;
    }
};

class LoadConfigHandler : public CefV8Handler
{
    IMPLEMENT_REFCOUNTING(LoadConfigHandler);

public:
    bool Execute(
        const CefString& Name,
        CefRefPtr<CefV8Value> Object,
        const CefV8ValueList& Arguments,
        CefRefPtr<CefV8Value>& Retval,
        CefString& Exception) override
    {
        Retval = CefV8Value::CreateBool(false);

        std::string File;

        if (Arguments.size() == 0)
        {
            std::ifstream In(std::filesystem::path(ConfigDirectory).append("default.json"));

            if (!In.good())
            {
                return false;
            }

            In >> File;
        }
        else
        {
            if (Arguments.size() != 1)
            {
                return false;
            }

            if (!Arguments[0]->IsString())
            {
                return false;
            }
            
            File = Arguments[0]->GetStringValue().ToString();

            if (File.empty())
            {
                return false;
            }

            std::ifstream Config(std::filesystem::path(ConfigDirectory).append(File));

            if (!Config.good())
            {
                return false;
            }

            Config >> File;
        }

        Retval = CefV8Value::CreateString(File);

        return true;
    }
};

class SetValueHandler : public CefV8Handler
{
    IMPLEMENT_REFCOUNTING(SetValueHandler);

    static CefValue* CreateArray(CefV8Value* Value, CefValue* Dst)
    {
        auto Values = CefListValue::Create();
        Values->SetSize(Value->GetArrayLength());

        for (int N = 0; N < Value->GetArrayLength(); ++N)
        {
            assert(Values->SetValue(N, FromV8Value(Value->GetValue(N), Values->GetValue(N))));
        }

        Dst->SetList(Values);

        return Dst;
    }

    static CefValue* FromV8Value(CefV8Value* Value, CefValue* Dst)
    {
        if (Value->IsArray() ||
            Value->IsArrayBuffer())
        {
            return CreateArray(Value, Dst);
        }
        else if (Value->IsObject())
        {
            CefDictionaryValue::KeyList Keys;
            Value->GetKeys(Keys);

            auto Dict = CefDictionaryValue::Create();
            
            for (const auto& Key : Keys)
            {
                assert(Dict->SetValue(Key, CefValue::Create()));
                assert(Dict->SetValue(Key, FromV8Value(Value->GetValue(Key), Dict->GetValue(Key))));
            }

            Dst->SetDictionary(Dict);
        }
        else if (Value->IsInt())
        {
            Dst->SetInt(Value->GetIntValue());
        }
        else if (Value->IsBool())
        {
            Dst->SetBool(Value->GetBoolValue());
        }
        else if (Value->IsString())
        {
            Dst->SetString(Value->GetStringValue());
        }
        else if (Value->IsDouble())
        {
            Dst->SetDouble(Value->GetDoubleValue());
        }
        else if (Value->IsUInt())
        {
            Dst->SetInt(Value->GetIntValue());
        }
        else if (Value->IsUndefined())
        {
            Dst->SetNull();
        }
        else if (Value->IsNull())
        {
            Dst->SetNull();
        }
        else if (Value->IsDate())
        {
            Dst->SetDouble(Value->GetDateValue().GetDoubleT());
        }
        else if (Value->IsFunction())
        {
            Dst->SetString(Value->GetFunctionName());
        }
        else
        {
            if (Dst->IsValid())
            {
                return CreateArray(Value, Dst);
            }
            Dst->SetNull();
        }
        return Dst;
    }

public:
    SetValueHandler(CefRefPtr<CefV8Context> Context)
        : Context{ Context }
    {}

    bool Execute(
        const CefString& Name,
        CefRefPtr<CefV8Value> Object,
        const CefV8ValueList& Arguments,
        CefRefPtr<CefV8Value>& Retval,
        CefString& Exception) override
    {
        if (Arguments.size() != 2)
        {
            return false;
        }

        if (!Arguments[0]->IsString())
        {
            return false;
        }

        if (Context)
        {
            auto Msg = CefProcessMessage::Create("SetValue");

            auto Args = Msg->GetArgumentList();
            {
                assert(Args->SetSize(2));
                Args->SetValue(0, FromV8Value(Arguments[0], Args->GetValue(0)));
                Args->SetValue(1, FromV8Value(Arguments[1], Args->GetValue(1)));
            }

            Context->GetFrame()->SendProcessMessage(CefProcessId::PID_BROWSER, Msg);
        }

        return true;
    }

private:
    CefRefPtr<CefV8Context> Context;
};

class UiApp : 
    public CefApp, 
    public CefRenderProcessHandler, 
    public CefLifeSpanHandler
{
    IMPLEMENT_REFCOUNTING(UiApp);

public:
    virtual void OnBrowserCreated(
        CefRefPtr<CefBrowser> Browser,
        CefRefPtr<CefDictionaryValue> ExtraInfo) override 
    {
        CurrentBrowser = Browser;
    }

    virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> Browser) override 
    {
        if (CurrentBrowser == Browser)
        {
            CurrentBrowser = nullptr;
        }
    }

    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override
    {
        return this;
    }

    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override
    { 
        return nullptr; 
    }

    virtual void OnContextCreated(
        CefRefPtr<CefBrowser> Browser,
        CefRefPtr<CefFrame> Frame,
        CefRefPtr<CefV8Context> Context) override
    {
        auto Global = Context->GetGlobal();
        assert(Global->SetValue("SaveConfig", CefV8Value::CreateFunction("SaveConfig", new SaveConfigHandler()), V8_PROPERTY_ATTRIBUTE_NONE));
        assert(Global->SetValue("LoadConfig", CefV8Value::CreateFunction("LoadConfig", new LoadConfigHandler()), V8_PROPERTY_ATTRIBUTE_NONE));
        assert(Global->SetValue("DeleteConfig", CefV8Value::CreateFunction("DeleteConfig", new DeleteConfigHandler()), V8_PROPERTY_ATTRIBUTE_NONE));
        assert(Global->SetValue("SetValue", CefV8Value::CreateFunction("SetValue", new SetValueHandler(Context)), V8_PROPERTY_ATTRIBUTE_NONE));
    }

private:
    CefRefPtr<CefBrowser> CurrentBrowser;
};

int main(int argc, char* argv)
{
    if (!std::filesystem::exists(ConfigDirectory))
    {
        std::error_code Ec;

        if (!std::filesystem::create_directories(ConfigDirectory, Ec))
        {
            std::cerr << "Failed to create config directory: " << Ec.message();
        }
    }

    return CefExecuteProcess(CefMainArgs(nullptr), new UiApp(), nullptr);
}