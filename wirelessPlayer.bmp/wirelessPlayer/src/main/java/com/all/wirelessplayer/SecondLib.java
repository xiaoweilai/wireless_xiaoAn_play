package com.all.wirelessplayer;

/**
 * Created by John on 15-8-6.
 */
public class SecondLib {
    // Native implementation
    static {
        System.loadLibrary("SecondLib");
    }
    //int array
    public static native int[] intMethod();
    //string array
    public static native String[] stringMethod();
}
