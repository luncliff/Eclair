# Java 빌드 추가 및 JUnit 5로 테스트하기

### Build

Gradle Plugin의 충돌 문제로 인해 빌드 구성은 C++(cpp.gradle)와 JNI(build.gradle)로 분리하여 진행함

```bash
gradle -b cpp.gradle    codegen build
gradle -b build.gradle  build
```

### Test

```bash
gradle -b cpp.gradle  test 
gradle -b build.gradle test
```

Java 테스트가 종료된 후, ./build/reports/tests/test/index.html 경로에 JUnit 테스트 보고서가 생성됨.

```ps1
Invoke-Item .\build\reports\tests\test\index.html
```
