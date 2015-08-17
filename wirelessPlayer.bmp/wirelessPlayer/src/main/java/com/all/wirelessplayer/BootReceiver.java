package com.all.wirelessplayer;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.NetworkInfo;
import android.net.NetworkInfo.State;
import android.net.wifi.WifiManager;
import android.os.Parcelable;

public class BootReceiver extends BroadcastReceiver {  
	private MyAPP myApp;
    @Override  
    public void onReceive(Context context, Intent intent) {  
        if(intent.getAction().equals("android.intent.action.BOOT_COMPLETED")) {     // boot  
            Intent intent2 = new Intent(context, SDLActivity.class);   
            intent2.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);  
            context.startActivity(intent2);  
        } else if (WifiManager.NETWORK_STATE_CHANGED_ACTION.equals(intent.getAction())) {  
         Parcelable parcelableExtra = intent.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO);  
         if (null != parcelableExtra) {  
             NetworkInfo networkInfo = (NetworkInfo) parcelableExtra;  
             State state = networkInfo.getState();  
             boolean isConnected = state == State.CONNECTED;
                 if (isConnected) {
//                	 myApp = (MyAPP)(((Activity) MainActivity.mContext).getApplication());
//                	 myApp.getHandler().sendEmptyMessage(2);
                 } else {
   
                 }  
             }  
         }  
    }  
}  