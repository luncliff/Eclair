# Java 빌드 추가 및 JUnit 5로 테스트하기

## 준비

앞서, JNI 인터페이스를 지원하는 DLL을 만들었으니, 이제 Java 부분을 작성할 차례입니다.

### 프로젝트 구조

기본적으로 Java 코드들은 [Maven Standard Directory Layout](http://maven.apache.org/guides/introduction/introduction-to-the-standard-directory-layout.html)을 따라 배치하고, 이 프로젝트를 위한 [`build.gradle`](../build.gradle), [`settings.gradle`](../settings.gradle)을 둘 것입니다.

```console
$ tree -L 2 .
.
├── cpp.gradle
├── ...
├── build.gradle
├── settings.gradle
└── src
    ├── main
    └── test
```

cpp.gradle 을 재사용하지 않는 이유는 `cpp-library` 플러그인과 `java` 플러그인을 동시에 사용할 수 없기 때문입니다.

결과적으로 이 프로젝트의 빌드 절차는 C++ 프로젝트를 먼저 빌드하고, Java 프로젝트를 빌드, 테스트하는 순서로 진행될 것입니다.

```
gradle -b cpp.gradle    codegen build test
gradle -b build.gradle  build test
```

### Dependency 정의

최종적으로는 DLL을 사용하는 jar 파일을 만드는 것이 목표이므로, `plugins` 에는 `java-libary`가 함께 명시되어야 합니다.

```gradle
plugins {
    id 'java'
    id 'java-library'
}
```

다음으로 필요한 [모듈들을 획득할 수 있도록 `repositories`을 지정](https://docs.gradle.org/6.0.1/userguide/core_dependency_management.html)해주고, [`dependencies`](https://docs.gradle.org/6.0.1/userguide/declaring_dependencies.html#sec:what-are-dependency-configurations)를 명시합니다.


```gradle
// ... plugins ...

repositories {
    jcenter()
}

dependencies {
    api 'org.apache.commons:commons-math3:3.6.1'
    // https://github.com/google/guava#adding-guava-to-your-build
    implementation 'com.google.guava:guava:28.0-jre'

    testImplementation 'org.junit.jupiter:junit-jupiter-api:5.5.1'
    testRuntimeOnly 'org.junit.jupiter:junit-jupiter-engine:5.5.1'
}
```

`api`는 전파되어야 하는(Public) 의존성, `implementation`은 내부적으로만 사용하는(Private) 의존성에 사용합니다.
[문서](https://docs.gradle.org/6.0.1/userguide/declaring_dependencies.html#sec:what-are-dependency-configurations)에 따르면 좀 더 상세하게 사용하는 방법도 있는 것 같지만...


![](https://docs.gradle.org/6.0.1/userguide/img/java-library-ignore-deprecated-main.png)

**그만 알아보도록 하겠습니다**.

이 프로젝트는 핵심 부분은 이미 C++로 구현하고, 그 위에 아주 얇은 Java 바인딩을 지원하는 상황을 가정하므로 불필요한 것 같습니다.

Java 코드의 전체 의존성을 확인해야 하는 경우라면 `dependencies` Task를 실행하는 것으로 쉽게 확인할 수 있습니다.

```
gradle -b ./build.gradle dependencies
```

## Java SourceSet 정의

### Java (Optional)

Java 호환성은 의외로 민감한 문제일 수 있으니 명시하는게 좋을 것 같습니다.

```gradle
// https://docs.gradle.org/6.0.1/javadoc/org/gradle/api/JavaVersion.html
java {
    sourceCompatibility = JavaVersion.VERSION_11
    targetCompatibility = JavaVersion.VERSION_11
}
```

### SourceSets

이 저장소에 특별히 리소스가 있는 것은 아니지만, 경로 설정이 가능한지 확인하는 차원에서 `sourceSets`를 정해주겠습니다.
기본 값들은 [The Java Plugin 문서의 Project Layout 부분](https://docs.gradle.org/6.0.1/userguide/java_plugin.html#sec:java_project_layout)에서 확인할 수 있습니다.

이 저장소에서는 아무 역할도 없는 파일 3개를 사용하고 있습니다.
전부 [Gradle의 기본 Java Library 템플릿](https://guides.gradle.org/building-java-libraries/)을 약간 변경한 것으로, 아래에서 Test 결과를 확인한 다음 기본 템플릿도 만들어보기를 권합니다.

* src/main/java
    * [Module.java](../src/main/java/dev/luncliff/Module.java)
    * [Module2.java](../src/main/java/dev/luncliff/Module2.java)
* src/test/java
    * [ModuleTest.java](../src/test/java/dev/luncliff/ModuleTest.java)

```gradle
sourceSets {
    main {
        java {
            srcDirs = ['src/main/java']
        }
        resources {
            srcDirs = ['src/resources']
        }
    }
    test {
        java {
            srcDirs = ['src/test/java']
        }
        resources {
            srcDirs = ['src/resources']
        }
    }
}
```

### 빌드 해보기

여기까지 작성된 시점에, 터미널에서 Gradle 명령으로 빌드/테스트가 가능해야 합니다.

```console
$ gradle build test --info
...
```

JUnit5 덕분에, Java 테스트가 종료된 이후로는 `build/reports/tests/test/index.html` 경로에 JUnit 테스트 보고서가 생성됩니다.

`sourceSets`만 정의하였을 뿐, `test`는 정의하지 않았으므로 어떤 테스트도 실행되지 않았다는 (0 TestCase) 내용이어야 합니다.

```ps1
Invoke-Item .\build\reports\tests\test\index.html
```

```bash
open ./build/reports/tests/test/index.html 
```

## Test 정의

### JUnit 5 명시

> 이 프로젝트에서 JUnit 5를 쓰는데는 특별한 이유는 없습니다. 단지 [사용자 가이드 문서](https://docs.gradle.org/6.0.1/userguide/java_testing.html#using_junit5)에서 설명하는 것을 따라했을 뿐입니다.

우선, CI 환경에서 테스트 로그가 없으면 허전할테니, 테스트 과정에서 로그가 C 표준 스트림으로 출력되도록 해주겠습니다.

```gradle
dependencies {
    // ...
    testImplementation 'org.junit.jupiter:junit-jupiter-api:5.5.1'
    testRuntimeOnly 'org.junit.jupiter:junit-jupiter-engine:5.5.1'
}

test {
    useJUnitPlatform()

    // https://blog.hkwon.me/gradle-test-task-logging/
    testLogging {
        events "passed", "skipped", "failed"
        showStandardStreams = true
        showCauses = true
        showExceptions = true
        showStackTraces = true
        exceptionFormat = 'full'
    }
    // ...
}
```

### java.library.path

이 프로젝트는 C++ DLL을 build의 하위폴더에 생성하기 때문에, 설치하지 않은 상태로 실행해보기 위해선 `java.library.path`에 경로를 추가해줄 필요가 있습니다.
build 폴더에는 현재 플랫폼에 따라 서로 다른 폴더 이름이 사용되므로, 현재 시스템을 확인하여 [`systemProperty`](http://dorbae.blogspot.com/2015/04/java-system-properties.html)를 변경해줍니다.


```gradle
import org.gradle.internal.os.OperatingSystem
import java.nio.file.Paths

// https://docs.gradle.org/6.0.1/dsl/org.gradle.api.tasks.testing.Test.html
test {
    // ...
    def system = OperatingSystem.current()
    def dirname = Paths.get("$project.buildDir", "lib/main/debug").toString()

    // notifty the library path
    if (system.isWindows()){
        systemProperty 'java.library.path', Paths.get(dirname, "windows").toString()
    }
    if (system.isMacOsX()){
        systemProperty 'java.library.path', Paths.get(dirname, "macos").toString()
    }
    if (system.isLinux()){
        systemProperty 'java.library.path', Paths.get(dirname, "linux").toString()
    }
    // ...
}
```

앞서 C++ 유닛 테스트를 실행할 때 환경 변수(`environment`)를 설정하여 실행했는데, 여기서도 같은 방식으로 설정할 수 있습니다.

```gradle
test {
    // ...
    // system env variable
    environment.put("TEST", "true")
    environment.put("BUILD_DIR", project.buildDir.toString())
}
```

결과적으로는 [build.gradle](../build.gradle)과 같은 내용이 될 것입니다.

이제 테스트를 실제로 수행한 뒤에, 결과를 확인해보면 [ModuleTest.java](../src/test/java/dev/luncliff/ModuleTest.java)의 `checkEnvVar`를 수행한 결과가 기록되어야 합니다.

```console
$ gradle assemble test

> Configure project :
zip: eclair-0.4

Deprecated Gradle features were used in this build, making it incompatible with Gradle 7.0.
Use '--warning-mode all' to show the individual deprecation warnings.
See https://docs.gradle.org/6.0.1/userguide/command_line_interface.html#sec:command_line_warnings

BUILD SUCCESSFUL in 1s
6 actionable tasks: 1 executed, 5 up-to-date

$ open ./build/reports/tests/test/index.html 
```

> [DSL 참고 문서](https://docs.gradle.org/6.0.1/dsl/org.gradle.api.tasks.testing.Test.html)를 보면 jacoco 플러그인도 함께 사용할 수 있는 것 같은데, 기회가 된다면 SonarQube 플러그인과 함께 사용해봐야겠습니다.

