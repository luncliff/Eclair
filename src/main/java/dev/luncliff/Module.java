package dev.luncliff;

import java.lang.System;

public class Module {
    static{
        System.loadLibrary("adapter");
    }

    public static native boolean isTestMode();
    public static native byte[] makeBlob(int size);
}
