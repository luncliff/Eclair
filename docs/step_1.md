# Gradle로 C++ 라이브러리 만들기

## 준비

### Gradle 실행해보기

우선 JDK와 Gradle이 모두 준비되었는지 확인해봐야겠습니다.
ready.gradle 파일을 만든 뒤, 아래와 같은 내용을 작성합니다.

```gradle
// ready.gradle
import org.gradle.internal.os.OperatingSystem

def currentSystem = OperatingSystem.current()
println "using System: $currentSystem"

def javaDir = "$System.env.JAVA_HOME"
println "using JAVA_HOME: $javaDir"
```

Mac 환경이라면, 아래와 비슷한 내용이 출력되어야 합니다.

```console
$ gradle -b ./ready.gradle 
Starting a Gradle Daemon (subsequent builds will be faster)

> Configure project :
using System: Mac OS X 10.15.4 x86_64
using JAVA_HOME: /Users/luncliff/.sdkman/candidates/java/current

> Task :help

Welcome to Gradle 6.0.1.
...
```

`println`에 의해 출력되는 using ... 으로 시작되는 부분 이외에도 내용이 많습니다.
눈여겨 볼 부분은 이 부분입니다.

```console
Welcome to Gradle 6.0.1.

To run a build, run gradle <task> ...

To see a list of available tasks, run gradle tasks

...
```

Gradle 은 기본적으로 Groovy 스크립트로, 간단한 형태로 함수를 호출하고 실행중인 시스템에 변화를 일으킬 수 있습니다.
복잡한 일들은을 Task로 정의하여 수행하며, Plugin은 기본적으로 이런 Task들을 미리 만들어둔 것입니다.
예를 들어, 안드로이드 개발 시에 널리 쓰이는 [Android Gradle Plugin](https://developer.android.com/studio/releases/gradle-plugin)은 "안드로이드 시스템에 대상으로 빌드할 때 필요한 Gradle Task들의 집합" 정도로 표현할 수 있겠습니다.

### Plugin 준비

C++을 사용하는 Java 모듈을 개발하는 것이 목표이므로, 먼저 Gradle 플러그인 중에서 C++와 관련된 플러그인들을 가져올 것입니다.

참고로, `plugins`는 `buildscript`와 함께 가장 앞에 배치되어야 합니다. 여기서는 `buildscript`는 사용하지 않으므로, `import` 문 바로 아래에 배치하면 되겠습니다.

```gradle
// ready.gradle
import java.nio.file.Paths

plugins {
    id 'cpp-library'
    id 'cpp-unit-test'
}
```

참고로, 플러그인 관련 오류가 출력되면 저는 가장 먼저 [저장소](https://docs.gradle.org/current/userguide/declaring_repositories.html#sec:declaring_public_repository) 설정을 바꿔보곤 합니다.

```gradle
repositories {
    jcenter()
}
```

전부 합치면 대략 이런 내용이 됩니다. 

```gradle
// ready.gradle
import org.gradle.internal.os.OperatingSystem
import java.nio.file.Paths

plugins {
    id 'cpp-library'
    id 'cpp-unit-test'
}

repositories {
    jcenter()
}

def currentSystem = OperatingSystem.current()
println "using System: $currentSystem"

def javaDir = "$System.env.JAVA_HOME"
println "using JAVA_HOME: $javaDir"
```

이대로 tasks 명령을 실행해보면, `assemble`(Build), `runTest`(Verification)와 같은 Task가 추가된 것을 확인할 수 있습니다. C++ 플러그인에 의해서 추가한 것들입니다.

```console
$ gradle -b ./ready.gradle tasks

> Task :tasks

------------------------------------------------------------
Tasks runnable from root project
------------------------------------------------------------

Build tasks
-----------
assemble - Assembles the outputs of this project.
...


Verification tasks
------------------
check - Runs all checks.
runTest - Executes C++ unit tests.
...
```

물론 스크립트 내에서 직접 task를 정의할수도 있습니다.
Java가 설치된 폴더에 `include`폴더가 있는지 확인해보도록 하겠습니다.

```gradle
// ...
def javaDir = "$System.env.JAVA_HOME"

task hello(type:Exec) {
    commandLine 'ls', Paths.get("$javaDir")
}
```

아래와 같이 출력된다면 이제 사전 준비와 연습은 마무리 된 것입니다.

```console
$ gradle -b ./ready.gradle hello

> Task :hello
bin
conf
include
jmods
legal
lib
release

BUILD SUCCESSFUL in 1s
```

## library 정의

하나의 .gradle 파일은 하나의 프로젝트를 정의하며, 오직 라이브러리만을 만들 수 있습니다.

```gradle
plugins {
    id 'cpp-library'
}

// https://docs.gradle.org/6.0.1/dsl/org.gradle.language.cpp.CppLibrary.html
library {
    // ...
}

library {
    // ... 한번 더 정의하면, 덮어쓰기가 되어버린다 ...
}
```

[Reference 문서](https://docs.gradle.org/6.0.1/dsl/org.gradle.language.cpp.CppLibrary.html)에서는 여러가지를 설명하고 있으나, 여기서는 5가지 속성을 정의할 것입니다. 

* `baseName`  
    Java 에서 `System.loadLibrary`에 제공할 이름을 말합니다. 저장소의 이름을 그대로 사용하겠습니다.
* `linkage`  
    Java 에서 `System.loadLibrary`를 하려면 shared 여야 합니다.
* `source`  
    복잡하지 않도록 JNI를 위한 소스파일은 한곳에 모아둡니다. Convention에 따라 여기서는 `src/main/cpp` 폴더를 사용합니다.
* `publicHeaders`  
    플러그인에서는 오직 하나의 폴더만 노출할 수 있도록 제한하고 있습니다. Convention에 따라 여기서는 `src/main/public` 폴더를 사용합니다.
* `privateHeaders`  
    다행히 이 부분은 여럿을 정의할 수 있습니다.
    JNI 프로젝트이므로, jni.h 파일을 찾을 수 있도록 하려면 그에 맞는 경로를 제공해야 합니다.


```gradle
import java.nio.file.Paths

plugins {
    id 'cpp-library'
}

library {
    baseName = 'eclair' 

    linkage = [Linkage.SHARED] 

    source.from \
        file('src/main/cpp')

    publicHeaders.from \
        file('src/main/public')

    privateHeaders.from \
        file("$javaDir\\include"), \
            file("$javaDir\\include\\win32"), file("$javaDir\\include\\darwin"), file("$javaDir\\include\\linux"), \
        file('src/main/cpp'), \
        file('src/main/headers')
}
```

내용은 복잡하지 않은데, 몇몇 부분은 검색해볼 가치가 있을 것입니다.

* [`file` 함수](https://docs.gradle.org/current/userguide/working_with_files.html#sec:locating_files). 입력에 따라 동작이 달라지는데, 지금처럼 문자열을 사용한 경우는 프로젝트 Root를 기준으로 상대 경로를 만들어줍니다.
* [`\\`, `/`](https://docs.oracle.com/javase/8/docs/api/java/io/File.html) 혼용: 다행스럽게도 두가지 모두 사용할 수 있습니다!

### Linkage

링킹이 필요없다면 좋겠지만, 그렇지 않다면 해당 파일을 전달해줘야 합니다.
Gradle Task들을 실행할 때 DYLD_LIBRARY_PATH, LD_LIBRARY_PATH, PATH에서 탐색할 수 있도록 환경변수를 조작해준다면 큰 문제없이 사용할 수 있을 것입니다.

`getLinkLibraryName`은 aaa.dll, libaaa.so 와 같이 플랫폼에 맞는 DLL 확장자를 붙여주는 역할을 합니다.

```gradle
tasks.withType(LinkSharedLibrary).configureEach {
    linkerArgs = [
        currentSystem.getLinkLibraryName("aaa")
    ]
}
```

플랫폼에 따라서는 다르게 적용해야 한다면, 아래처럼 사용할수도 있습니다.

```gradle
tasks.withType(LinkSharedLibrary).configureEach {
    linkerArgs.addAll targetPlatform.map { platform ->
        if (platform.operatingSystem.isWindows()){
            def libname = currentSystem.getLinkLibraryName('ws2_32');
            return [libname]
        }
        return []
    }
}
```

fileTree를 사용하면 패턴매칭을 사용할수도 있습니다.
하지만 Mac Dylib은 사용여부와 관계없이 링킹시에 명시된 모든 모듈에 의존성을 가지게 된다는 점에 주의할 필요가 있겠습니다.

```gradle
tasks.withType(LinkSharedLibrary).configureEach {
    // glob library in specific folder
    if (currentSystem.isMacOsX()){
        def pattern = currentSystem.getLinkLibraryName('*.dylib')
        def prebuilts = fileTree(dir: "$javaDir\\lib", includes: [pattern])
        for (file in prebuilts.getFiles()){
            println "module: ${file.getName()}"
        }
        libs.from prebuilts
    }
}
```

여기서 `def`는 지역변수를 만든 것입니다. 이를 명시하지 않으면 앞서 library 에서 `baseName` property를 변경한 것과 같은 의미가 됩니다.
그러면 존재하지 않는 property를 변경하려고 한다는 오류가 출력될 것입니다.

### Compile

소스코드를 위한 헤더(인터페이스) 파일들을 위한 경로, 링커를 위한 라이브러리들을 목록을 제공하였으므로,
이제 컴파일러 옵션을 정의할 때입니다.

```gradle
tasks.withType(CppCompile).configureEach {
    println "configure task: ${getName()}"

    // CppCompile properties
    positionIndependentCode = true
    debuggable = true
    optimized = false

    // compile-time macro
    macros.put('DEBUG', '1')

    // compiler options
    compilerArgs.addAll toolChain.map { NativeToolChain toolChain ->
        if (toolChain in Clang) {
            return ['-Wall', '-std=c++17', '-stdlib=libc++']
        }
        if (toolChain in Gcc) {
            return ['-Wall', '-std=gnu++17']
        }
        if (toolChain in VisualCpp) {
            return ['/W4', '/std:c++17', '/utf-8']
        }
        return [] // unknown compiler
    }
}
```

[CppCompile은 변경 가능한 property가 그리 많지 않습니다](https://docs.gradle.org/6.0.1/dsl/org.gradle.language.cpp.tasks.CppCompile.html#N2ED58).
`source`, `includes` 와 같은 속성들은 이미 `library`에서 지정한 내용을 공유하기 때문에, 여기서는 간단히 매크로와 경고, C++ 표준 등을 추가하였습니다.

`assemble` 명령으로 빌드를 수행하고 나면 debug 모드에 해당하는 결과물만 생성된 것을 볼 수 있습니다. [이는 assemble Task가 release 관련 Task를 수행하지 않기 때문입니다.](https://docs.gradle.org/current/userguide/cpp_library_plugin.html#sec:cpp_library_tasks)

```console
$ gradle -b ./cpp.gradle assemble
$ tree ./build
./build
├── lib
│   └── main
│       └── debug
│           └── libeclair.dylib
├── obj
│   └── main
│       └── debug
│           └── ago4urd9ewsvza3rxb2rha2r
│               └── adapter.o
└── tmp
    ├── compileDebugCpp
    │   ├── options.txt
    │   └── output.txt
    └── linkDebug
        ├── options.txt
        └── output.txt

10 directories, 6 files
```

그러니 Release 설정으로 빌드된 결과물이 필요하다면 `assembleRelease`를 별도로 실행해줄 필요가 있습니다.

[![](https://docs.gradle.org/current/userguide/img/cpp-shared-library-task-graph.png)](https://docs.gradle.org/current/userguide/cpp_library_plugin.html#sec:cpp_library_tasks)


## unitTest 정의

JNI 프로젝트처럼 순수한 C Runtime 환경이 아닌 환경에서 동작한다면 테스트의 중요성이 더욱 높아질 것입니다.
`cpp-unit-test` 플러그인을 사용해서 C++ 실행파일을 만들고, 기대한대로(혹은 명세에 따라서) 동작하는지 확인해볼 수 있습니다.

하나의 gradle 파일은 오직 하나의 테스트 프로그램(실행파일)을 생성할 수 있습니다.
[Gradle 공식 블로그에서는 Google Test를 사용했기 때문에](https://blog.gradle.org/introducing-the-new-cpp-plugins),
이 저장소에서는 Header-Only 테스트 프레임워크를 쓰는 경우를 상정하여 [Boost UT](https://github.com/boost-experimental/ut)를 사용해 볼 것입니다.


```gradle
// cpp.gradle
plugins {
    id 'cpp-library'
    id 'cpp-unit-test'
}

unitTest {
    baseName = 'eclair_test' 

    source.from file('test/cpp')

    privateHeaders.from \
        file('externals/ut/include'), \
        file('test/cpp')
}
```

* `baseName`  
    `library`를 정의할 때와 동일합니다.
* `source.from`  
    [test/cpp 폴더](../test/cpp) 경로만을 지정하여 있는 파일을 전부 사용하도록 하였습니다.  
    [문서](https://docs.gradle.org/6.0.1/dsl/org.gradle.language.cpp.tasks.CppCompile.html#org.gradle.language.cpp.tasks.CppCompile:source)에 따르면 원래 `source`는 read-only 이지만, [`ConfigurableFileCollection`](https://docs.gradle.org/6.0.1/javadoc/org/gradle/api/file/ConfigurableFileCollection.html)이므로 메서드 `from`을 호출함으로써 다수의 경로를 설정할 수 있습니다.
* `privateHeaders`  
    external / internal 파일들을 찾을 수 있도록 경로들을 지정합니다.

### Linkage

이번에도 `LinkExecutable`들에 대해서 링킹 설정을 해줘야 합니다.

```gradle
// test executable may have dependency ...
tasks.withType(LinkExecutable).configureEach {
    linkerArgs.addAll targetPlatform.map { platform ->
        if (platform.operatingSystem.isWindows()){
            return [ currentSystem.getLinkLibraryName('ws2_32') ]
        }
        return []
    }
}
```

### Runtime

실행 시점에 환경변수를 설정할 수 있습니다.

```gradle
tasks.withType(RunTestExecutable).configureEach {
    environment.put("TEST", "true")
}
```

그렇다면 `eclair` 라이브러리는 실행 시간에 이를 확인해볼 수 있겠군요.  
[src/main/cpp/adapter.cpp](../src/main/cpp/adapter.cpp).

```cpp
#include <cstdlib>

#include "adapter.h"

// JNI generated code
#include "dev_luncliff_Module.h"
#include "dev_luncliff_Module2.h"

bool is_test_mode() { return ::getenv("TEST") != nullptr; }

// ...
```

테스트 코드에서는 이 함수를 호출해보기만 하면 될 것입니다.  
[test/cpp/test_env.cpp](../test/cpp/test_env.cpp)

```cpp
#include <boost/ut.hpp>

#include "adapter.h"

namespace testing = boost::ut;

void test_env(int, char*[]) {
    using namespace testing;

    "env"_test = []() { testing::expect(is_test_mode()); };
}
```

## Codegen Task 정의

`"dev_luncliff_Module.h"`파일을 보고 생각나셨을지 모르겠습니다만, JNI 라이브러리를 만들기 위해서는 `native` 메서드가 선언된 java 파일에 해당하는 .h 파일을 생성해야 합니다.
`javac`는 다수의 java 파일로부터 .h를 생성할 수 있습니다.

```console
$ javac -h ./src/main/headers ./src/main/java/dev/luncliff/Module.java 
```

지금이야 터미널에서 명령 한번이면 충분하겠지만, java 파일이 많아진다면 힘들어질 것입니다. 
이 부분은 별도의 Gradle Task로 정의해보겠습니다.
결과적으로, 아래와 같이 한번의 명령으로 JNI 파일 생성과 빌드를 단계적으로 수행하게 될 것입니다.

```console
$ gradle -b ./cpp.gradle codegen assemble
```

[`Exec`타입](https://docs.gradle.org/6.0.1/dsl/org.gradle.api.tasks.Exec.html)의 [`task`](https://docs.gradle.org/6.0.1/dsl/org.gradle.api.Task.html) codegen을 정의합니다.  
상대 경로를 사용하기 때문에 `workingDir` 속성을 명시하고,
터미널에서 실행할 명령을 그대로 `commandLine` 속성에 대입합니다.

```gradle
import java.nio.file.Paths

// ...

task codegen(type:Exec) {
    // change the working directory
    workingDir = project.projectDir
    println "running 'javac' in $workingDir ..."

    commandLine 'javac', '-h', "src/main/headers", \
        Paths.get("src/main/java", "dev/luncliff/Module.java"), \
        Paths.get("src/main/java", "dev/luncliff/Module2.java")
}
```

이를 실행하면 `library`의 `privateHeaders` 중 하나인 `src/main/headers`에 헤더 파일들을 생성하게 될 것입니다.

```console
$ rm -rf ./src/main/headers/
$ gradle -b ./cpp.gradle codegen
running 'javac' in /Users/luncliff/dev/eclair ...
...
$ tree ./src/main/headers/
./src/main/headers/
├── dev_luncliff_Module.h
└── dev_luncliff_Module2.h

0 directories, 2 files
```

## 결과 확인

최종적으로, [cpp.gradle](../cpp.gradle)의 사용법은 2 가지로 정리됩니다.
충분히 실험해보고 다음 단계로 넘어가기를 바랍니다.

### Codegen / Assemble

하나는 JNI 헤더를 생성하고 C++ DLL을 빌드하는 것.

```console
$ gradle -b ./cpp.gradle codegen assembleDebug assembleRelease

> Configure project :
using System: ...
...
```

### Run Test

다른 하나는 이 DLL에 대한 테스트를 수행하는 것입니다.

```console
$ gradle -b ./cpp.gradle runTest

> Configure project :
using System: ...
...

> Task :runTest
All tests passed (1 asserts in 1 tests)

```

