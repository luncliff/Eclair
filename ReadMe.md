
# C++ Gradle in Kor

[![Build Status](https://dev.azure.com/luncliff/personal/_apis/build/status/luncliff.cpp-gradle-in-kor?branchName=master)](https://dev.azure.com/luncliff/personal/_build/latest?definitionId=31&branchName=master)
[![](https://img.shields.io/badge/Gradle-5.6%2B-%2302303A)](https://gradle.org)

[Gradle](https://gradle.org/features/)만으로 JNI 모듈을 빌드/테스트/패키징 하는 템플릿 프로젝트

### References

* 책
    * Gradle 철저 입문 - 김완섭 옮김, 길벗 출판사
    * 안드로이드의 모든 것 NDK - 고현철,전호철 지음, 한빛미디어
* JNI: Java Native Interface
    * https://docs.oracle.com/en/java/javase/11/docs/specs/jni/index.html
    * http://nerdposts.blogspot.com/2010/10/jni-mac-os-x-simple-sample.html?m=1
    * https://support.microsoft.com/ko-kr/help/222092
* Apache Maven
    * http://maven.apache.org/guides/introduction/introduction-to-the-standard-directory-layout.html
* Gradle 공식 문서, 예제
    * https://guides.gradle.org/building-cpp-libraries/
    * https://docs.gradle.org/current/userguide/building_cpp_projects.html
    * https://github.com/gradle/native-samples
* Gradle 사용법, Tips
    * https://serviceorientedarchitect.com/how-to-combinejoin-file-paths-in-gradlegroovy/
* JUnit5
    * https://junit.org/junit5/docs/current/user-guide/
    * https://developer.ibm.com/tutorials/j-introducing-junit5-part1-jupiter-api/

### Code Organization

기본적으로 Gradle로 빌드되는 Sub-project로 가정하며, [Apache Maven의 구성 방법](http://maven.apache.org/guides/introduction/introduction-to-the-standard-directory-layout.html)을 기본으로 하되, 가능한 C++ 프로젝트들과 유사하게 재구성

## How To

### Setup

* JDK 11+
* Gradle 5.6+

#### Windows

윈도우 환경에서는 [Chocolatey](https://chocolatey.org/)를 사용

```ps1
choco install openjdk
choco install gradle
```

설치 이후 Gradle에서 요구하는 환경변수 `JAVA_HOME` 및 일반적으로 Java 툴킷에 필요한 `JDK_HOME`을 설정. (빌드에서는 `JAVA_HOME`만을 사용)

```ps1
Write-Output $env:JAVA_HOME
# C:\Program Files\OpenJDK\jdk-13.0.1
Write-Output $env:JDK_HOME
# C:\Program Files\OpenJDK\jdk-13.0.1
```

#### UNIX-like

[SDKMan](https://sdkman.io/)을 사용. 아래의 내용은 프로젝트 목적을 고려해 [설치 과정](https://sdkman.io/install)을 요약한 것.

```console
user@host:~$ sudo apt install -y -qq unzip zip curl
user@host:~$ curl -s "https://get.sdkman.io" | bash
user@host:~$ source "/home/luncliff/.sdkman/bin/sdkman-init.sh"
```

Console 재시작.

```console
user@host:~$ sdk version
...
SDKMAN 5.7.4+362
```

JDK 설치. 설치 후, `JDK_HOME`, `JAVA_HOME` 환경변수 설정필요. 두 값은 같은 값을 사용하며, 모두 `javac` 프로그램의 위치를 사용해 획득 가능.

```console
user@host:~$ sdk install java 13.0.1-open
...
user@host:~$ javac --version
javac 13.0.1
user@host:~$ export JDK_HOME="$(dirname $(which javac))"
user@host:~$ export JAVA_HOME=${JDK_HOME}
```

이후 Gradle 설치 및 버전 확인

```console
user@host:~$ sdk install gradle 6.0.1
...
user@host:~$ gradle --version

------------------------------------------------------------
Gradle 6.0.1
------------------------------------------------------------

Build time:   2019-11-18 20:25:01 UTC
Revision:     fad121066a68c4701acd362daf4287a7c309a0f5
...
```

### Start

시작에 앞서, Gradle 공식 문서 2개를 확인할 것.

* https://docs.gradle.org/current/userguide/getting_started.html
* https://docs.gradle.org/current/userguide/building_cpp_projects.html

Gradle의 C++/Swift 예제 프로젝트들은 https://github.com/gradle/native-samples 에서 확인 가능. (버전 차이에 주의)

### Build

Gradle Plugin의 충돌 문제로 인해 빌드 구성은 C++(build-cpp.gradle)와 JNI(build-java.gradle)로 분리하여 진행함

```bash
gradle -b build-cpp.gradle  codegen build
gradle -b build-java.gradle build
```

### Test

```bash
gradle -b build-cpp.gradle  test 
gradle -b build-java.gradle test
```

Java 테스트가 종료된 후, ./build/reports/tests/test/index.html 경로에 JUnit 테스트 보고서가 생성됨.

```ps1
Invoke-Item .\build\reports\tests\test\index.html
```

### Packaging

개발 과정에서는 build 명령으로 충분하지만, [패키징에는 `assembleRelease`가 포함되어야 하므로](https://docs.gradle.org/6.0.1/userguide/cpp_library_plugin.html#sec:cpp_library_tasks) 다음과 같이 추가 빌드 이후 `distZip`을 사용해 zip 파일 생성.

```bash
gradle -b build-cpp.gradle  assembleDebug assembleRelease
gradle -b build-java.gradle build distZip
```

생성 결과는 'build/distributions'에 위치.

```console
PS C:\Users\luncl\cpp-gradle-in-kor> dir .\build\distributions

    Directory: C:\Users\luncl\cpp-gradle-in-kor\build\distributions

Mode                LastWriteTime         Length Name
----                -------------         ------ ----
-a----       11/24/2019   5:29 PM        5232640 cpp-gradle-in-kor-0.3.tar
-a----       11/24/2019   6:09 PM        8553158 cpp-gradle-in-kor-0.3.zip
```
