package dev.luncliff;

import java.nio.file.Paths;
import java.io.File;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.*;

// https://www.petrikainulainen.net/programming/testing/junit-5-tutorial-writing-our-first-test-class/
class ModuleTest {
     
    @BeforeAll
    static void beforeAll() {
        var platform = System.getProperty("os.name");
        System.out.println(platform);
    }
 
    @Test
    void checkEnvVar() {
        var description = "when env TEST is defined, must be 'true'";
        // see 'test' in build-java.gradle
        assertEquals("true", System.getenv("TEST"));
        assertTrue(Module.isTestMode(), description);
    }

}
