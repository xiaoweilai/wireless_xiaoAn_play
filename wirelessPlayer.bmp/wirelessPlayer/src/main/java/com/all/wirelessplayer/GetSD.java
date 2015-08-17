package com.all.wirelessplayer;

import java.io.File;

import android.os.Environment;
import android.os.StatFs;

public class GetSD {
	public static String getSDPath(){
		  File sdDir = null;
		  boolean sdCardExist = Environment.getExternalStorageState()
		  .equals(Environment.MEDIA_MOUNTED); //�ж�sd���Ƿ����
		  if (sdCardExist)
		  {
		  sdDir = Environment.getExternalStorageDirectory();//��ȡ��Ŀ¼
		  }else{
			  return null; 
		  }
//		  System.out.println(sdDir.toString());
		  return sdDir.toString();
		   
		 }
	public static long getSDFreeSize(){  
	     //ȡ��SD���ļ�·��  
	     File path = Environment.getExternalStorageDirectory();   
	     StatFs sf = new StatFs(path.getPath());   
	     //��ȡ������ݿ�Ĵ�С(Byte)  
	     long blockSize = sf.getBlockSize();   
	     //���е���ݿ������  
	     long freeBlocks = sf.getAvailableBlocks();  
	     //����SD�����д�С  
	     //return freeBlocks * blockSize;  //��λByte  
	     //return (freeBlocks * blockSize)/1024;   //��λKB  
	     return (freeBlocks * blockSize)/1024 /1024; //��λMB  
	   }
}
