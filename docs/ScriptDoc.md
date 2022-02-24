# Scripting Core

Register C++ native function with
```cpp
REGISTER_FUNCTION(someCppNativeFunctionPointer, desiredScopeName, parameterCountAsInt);
```
Maximum of five function arguments are allowed, and they have to be one of 
* std::string
* const char*
* float
* int
* bool

The function may return a value with one of the previous types

First compile a script with `unsigned __int32 CompileScript(const char* data); `
The returned value will be the identifier of the script.

Then, evaluate the script as needed with `
void EvaluateScript(unsigned __int32 scriptID);`

Finally, clean the script with `
void CleanScript(unsigned __int32 scriptID);`

Compile and runtime errors ate stored internally and may be retrieved with `
void GetError(const char* errorBuffer, size_t bufferSize);`

Sample script

```javascript
def oneFunction(array)
{
	var forloop;
	for(var i = 0; i < array.size(); i++)
		array[i] = i * 3+ 1;
	return array.size();
}

def other_function(someBool, other)
{
	var tempText;
	if (someBool) tempText = other + "TestText"
	if(!someBool) tempText = other - "OldTe";
	print(tempText);
}

var scriptGlobalVar = "OldTextBlock";

def execute()
{
	var array[];
	
	for (var i = 0; i <= 6; i++) {
		array[] = i;
	}
	print(array);
	
	print(oneFunction(array.copy()));
	print(array);
	print(oneFunction(array));
	print(array);
	
	array.unset();
	print(array.type());
	
	other_function(true, scriptGlobalVar);
	
	var testString = someCppNativeFunction(scriptGlobalVar, 3);
	print(testString);
	
	print(scriptGlobalVar);
}
```

Expected output:

```
{ 0, 1, 2, 3, 4, 5, 6 }
7
{ 0, 1, 2, 3, 4, 5, 6 }
7
{ 1, 4, 7, 10, 13, 16, 19 }
undefined
OldTextBlockTestText
OldTextBlockOldTextBlockOldTextBlock
OldTextBlock
```