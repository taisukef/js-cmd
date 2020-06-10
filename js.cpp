//-------------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//-------------------------------------------------------------------------------------------------------

#include <ChakraCore.h>
#include <ChakraCommon.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>

#define FAIL_CHECK(cmd)                     \
    do                                      \
    {                                       \
        JsErrorCode errCode = cmd;          \
        if (errCode != JsNoError)           \
        {                                   \
            printf("Error %d at '%s'\n",    \
                errCode, #cmd);             \
            return 1;                       \
        }                                   \
    } while(0)

using namespace std;

void setProperty(JsValueRef object, const char* propertyName, JsValueRef property) {
	JsPropertyIdRef propertyId;
	JsCreatePropertyId(propertyName, strlen(propertyName), &propertyId);
	JsSetProperty(object, propertyId, property, true);
}

void setCallback(JsValueRef object, const char* propertyName, JsNativeFunction callback, void *callbackState) {
	JsPropertyIdRef propertyId;
	JsCreatePropertyId(propertyName, strlen(propertyName), &propertyId);
	JsValueRef function;
	JsCreateFunction(callback, callbackState, &function);
	JsSetProperty(object, propertyId, function, true);
}

char* toString(JsValueRef resultJSString) {
    size_t stringLength;
    if (JsCopyString(resultJSString, nullptr, 0, &stringLength) != JsNoError) {
        return nullptr;
    }
    char* resultSTR = (char*)malloc(stringLength + 1);
    if (resultSTR == nullptr) {
        return nullptr;
    }
    if (JsCopyString(resultJSString, resultSTR, stringLength + 1, nullptr) != JsNoError) {
        free(resultSTR);
        return nullptr;
    }
    resultSTR[stringLength] = 0;
    return resultSTR;
}

int printValue(JsValueRef resultJSString) {
    char* resultSTR = toString(resultJSString);
    printf("%s", resultSTR);
    free(resultSTR);
    return 0;
}

JsValueRef log(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
	for (int i = 1; i < argumentCount; i++) {
		if (i > 1) {
			printf(" ");
		}
		JsValueRef stringValue;
		JsConvertValueToString(arguments[i], &stringValue);
        printValue(stringValue);
	}
	printf("\n");
	return JS_INVALID_REFERENCE;
}

char* concatArgv(int argc, char** argv) {
    unsigned int len = 0;
    for (int i = 1; i < argc; i++) {
        len += strlen(argv[i]) + 1;
    }
    char* buf = (char*)malloc(len);
    buf[0] = '\0';
    for (int i = 1; i < argc; i++) {
        strcat(buf, argv[i]);
        unsigned int off = strlen(buf);
        buf[off] = ' ';
        buf[off + 1] = '\0';
    }
    buf[len - 1] = '\0';
    printf("%s#\n", buf);
    return buf;
}

char* readTextFile(const char* fn) {
    FILE* fp = fopen(fn, "rb");
    if (fp == nullptr) {
        return nullptr;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return nullptr;
    }
    unsigned int len = ftell(fp);
    if (fseek(fp, 0L, SEEK_SET) != 0) {
        fclose(fp);
        return nullptr;
    }
    char* buf = (char*)malloc(len + 1);
    if (buf == nullptr) {
        fclose(fp);
        return nullptr;
    }
    buf[len] = '\0';
    unsigned int size = len;
    unsigned int off = 0;
    for (;;) {
        unsigned int size = fread(buf + off, 1, len - off, fp);
        if (size < 0) {
            fclose(fp);
            free(buf);
            return nullptr;
        }
        off += size;
        len -= size;
        if (!len) {
            break;
        }
    }
    fclose(fp);
    return buf;
}
void freeTextFile(char* s) {
    free(s);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("js-cmd usage)\n");
        printf("$ js 1+1\n");
        printf("$ js \"2*3\"\n");
        printf("$ js 0xff\n");
        printf("$ js \"p(2); console.log(1);\"\n");
        return 0;
    }

    unsigned currentSourceContext = 0;

    // Your script; try replace hello-world with something else
    // const char* script = "(()=>{ const x = 30; return `Hello World ${x ** 3}!`;})()";
    const char* script = argv[1]; // "10**2"
    // char* script = readTextFile(argv[1]); // from file
    // char* script = concatArgv(argc, argv); // *がファイル名として認識するなど使い勝手わるい
    
    // Create a runtime.
    JsRuntimeHandle runtime;
    FAIL_CHECK(JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &runtime));

    // Create an execution context.
    JsContextRef context;
    FAIL_CHECK(JsCreateContext(runtime, &context));

    // Now set the current execution context.
    FAIL_CHECK(JsSetCurrentContext(context));

    // Project method - console.log
	JsValueRef globalObject;
	JsGetGlobalObject(&globalObject);
	JsValueRef console;
	JsCreateObject(&console);
    /*
    JsPropertyIdRef property;
    JsCreatePropertyId("console", strlen("console"), &property);
    */
   /*
    JsValueRef symConsole;
    FAIL_CHECK(JsCreateString("console", strlen("console"), &symConsole));
    */

	JsPropertyIdRef propertyId;
    JsCreatePropertyId("console", strlen("console"), &propertyId);
	// JsSetProperty(globalObject, propertyId, valtest, true);
	// JsGetPropertyIdFromSymbol(symConsole, &propertyId);

    /* // test: property value
    JsValueRef valtest;
    FAIL_CHECK(JsCreateString("test", strlen("test"), &valtest));
    */

    // console.log funciton
	setCallback(console, "log", log, nullptr); 
	setProperty(globalObject, "console", console);

    // const p = console.log
	setCallback(globalObject, "p", log, nullptr);

    JsValueRef fname;
    FAIL_CHECK(JsCreateString("", strlen(""), &fname));

    JsValueRef scriptSource;
    FAIL_CHECK(JsCreateExternalArrayBuffer((void*)script, (unsigned int)strlen(script),
        nullptr, nullptr, &scriptSource));
    // Run the script.
    // FAIL_CHECK(JsRun(scriptSource, currentSourceContext++, fname, JsParseScriptAttributeNone, &result));
    JsValueRef result;
    JsParseScriptAttributes mode = JsParseScriptAttributeStrictMode;
    // JsParseScriptAttributes mode = JsParseScriptAttributeNone;
    if (JsRun(scriptSource, currentSourceContext++, fname, mode, &result) != JsNoError) {
    //if (JsRunScriptWithParserState(scriptSource, currentSourceContext++, fname, mode, JS_INVALID_REFERENCE, &result) != JsNoError) {
        // Get error message
        JsValueRef exception;
        FAIL_CHECK(JsGetAndClearException(&exception));

        // JsGetPropertyIdFromSymbol(symConsole, &propertyId);

        /* // show message
        JsPropertyIdRef messageName;
        JsCreatePropertyId("message", strlen("message"), &messageName);
        JsValueRef messageValue;
        // FAIL_CHECK(JsGetPropertyIdFromName(L"message", &messageName)); // for win
        FAIL_CHECK(JsGetProperty(exception, messageName, &messageValue));
        char* message = toString(messageValue);
        printf("%s\n", message);
        free(message);
        */
        /* // show exception == message
        JsValueRef exceptionJSString;
        FAIL_CHECK(JsConvertValueToString(exception, &exceptionJSString));
        char* message = toString(exceptionJSString);
        printf("%s\n", message);
        free(message);
        */

        // show stack trace
        JsPropertyIdRef stackId;
        JsCreatePropertyId("stack", strlen("stack"), &stackId);
        JsValueRef stackObject;
        FAIL_CHECK(JsGetProperty(exception, stackId, &stackObject));
        JsValueRef stackString;
        FAIL_CHECK(JsConvertValueToString(stackObject, &stackString));
        char* message = toString(stackString);
        printf("%s\n", message);
        free(message);
        
        return 1;
    }

    // Convert your script result to String in JavaScript; redundant if your script returns a String
    JsValueRef resultJSString;
    FAIL_CHECK(JsConvertValueToString(result, &resultJSString));

    // Project script result back to C++.
    char* resultSTR = toString(resultJSString);
    printf("%s\n", resultSTR);
    free(resultSTR);

    // Dispose runtime
    FAIL_CHECK(JsSetCurrentContext(JS_INVALID_REFERENCE));
    FAIL_CHECK(JsDisposeRuntime(runtime));

    // freeTextFile(script);
    return 0;
}
