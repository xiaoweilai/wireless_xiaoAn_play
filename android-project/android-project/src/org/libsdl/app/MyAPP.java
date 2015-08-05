package org.libsdl.app;


import android.app.Application;

public class MyAPP extends Application {  
	
	@Override  
    public void onCreate() {  
        super.onCreate();  
        CrashHandler crashHandler = CrashHandler.getInstance();  
        crashHandler.init(this);  
    }  
//	
//    // 共享变量  
//    private MyHandler handler = null;  
//      
//    // set方法  
//    public void setHandler(MyHandler mHandler) {  
//        this.handler = mHandler;  
//    }  
//      
//    // get方法  
//    public MyHandler getHandler() {  
//        return handler;  
//    }  
}  