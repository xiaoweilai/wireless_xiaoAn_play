package org.libsdl.app;

import java.io.BufferedOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.sql.Date;
import java.text.SimpleDateFormat;
import java.util.Enumeration;
import java.net.Inet4Address;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.view.WindowManager;
import android.widget.Toast;

public class PublicFunction {
private static int curImageIndex = 0;
	public static int toInt(byte[] bRefArr) {
	    int iOutcome = 0;
	    byte bLoop;

	    for (int i = 0; i < bRefArr.length; i++) {
	        bLoop = bRefArr[i];
	        iOutcome += (bLoop & 0xFF) << (8 * i);
	    }
	    return iOutcome;
	}
	
	public static byte[] toByteArray(int iSource, int iArrayLen) {
	    byte[] bLocalArr = new byte[iArrayLen];
	    for (int i = 0; (i < 4) && (i < iArrayLen); i++) {
	        bLocalArr[i] = (byte) (iSource >> 8 * i & 0xFF);
	    }
	    return bLocalArr;
	}
	
	public static Bitmap bytesToBimap(byte[] b){   
        if(b.length!=0){
            return BitmapFactory.decodeByteArray(b, 0, b.length);   
        } else {
            return null;   
        }   
  } 
	
	public static byte[] long2bytes(long num) {
		byte[] b = new byte[8];
		for (int i=0;i<8;i++) {
			b[i] = (byte)(num>>>(56-(i*8)));
		}
		return b;
	}
	
	public static long bytes2long(byte[] b) {
		long temp = 0;
		long res = 0;
		for (int i=0;i<8;i++) {
			res <<= 8;
			temp = b[i] & 0xff;
			res |= temp;
		}
		return res;
	}
	
	public static String getLocalIPAddress() throws SocketException{  
	    for(Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces();en.hasMoreElements();){  
	        NetworkInterface intf = en.nextElement();  
	        for(Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements();){  
	            InetAddress inetAddress = enumIpAddr.nextElement();  
	            if(!inetAddress.isLoopbackAddress() && (inetAddress instanceof Inet4Address)){  
	                return inetAddress.getHostAddress().toString();  
	            }  
	        }  
	    }  
	    return null;  
	}
	
	public static int[] getScreenWithAndHeight(Context context){
		WindowManager wm = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
		int wh[] = new int[2];
		int width = wm.getDefaultDisplay().getWidth();
		int height = wm.getDefaultDisplay().getHeight();
		wh[0] = width;
		wh[1] = height;
		System.out.println("wh[0] = " + wh[0] + "wh[1] = " + wh[1]);
		return wh;
	}
	
	public static void writeFile(Context mContext,String path,String content) {
		String firstpath = "bytesave";
		
		if(GetSD.getSDPath()!=null){
			firstpath=GetSD.getSDPath()+"/"+firstpath;
		}
		if(GetSD.getSDFreeSize()<1){
			Toast.makeText(mContext, "SD卡剩余空间不足，不能保存记录，请释放SD卡空间", Toast.LENGTH_LONG).show();
			return;
		}
		File idFile = new File(firstpath);
		if (!idFile.exists()) {
			idFile.mkdir();
		}
		String recPath = firstpath + "/"+path;
		idFile = new File(recPath);
		if (!idFile.exists()) {
			try {
				idFile.createNewFile();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		try {
			FileWriter fileWriter = new FileWriter(recPath, true);
			fileWriter.write(content);
			fileWriter.flush();
			fileWriter.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public static void writeFile(Context mContext,String path,byte[] b) {
		String firstpath = "bytesave";
		
		if(GetSD.getSDPath()!=null){
			firstpath=GetSD.getSDPath()+"/"+firstpath;
		}
		if(GetSD.getSDFreeSize()<1){
			Toast.makeText(mContext, "SD卡剩余空间不足，不能保存记录，请释放SD卡空间", Toast.LENGTH_LONG).show();
			return;
		}
		File idFile = new File(firstpath);
		if (!idFile.exists()) {
			idFile.mkdir();
		}
		String recPath = firstpath + "/"+path;
		idFile = new File(recPath);
		if (!idFile.exists()) {
			try {
				idFile.createNewFile();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		getFileFromBytes(b, recPath);
//		try {
//			FileWriter fileWriter = new FileWriter(recPath, false);
//			fileWriter.write(content);
//			
//			fileWriter.flush();
//			fileWriter.close();
//		} catch (IOException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
	}
	
	public static File getFileFromBytes(byte[] b, String outputFile) { 
        File ret = null; 
        BufferedOutputStream stream = null; 
        try {
           ret = new File(outputFile); 
           FileOutputStream fstream = new FileOutputStream(ret, true); 
           stream = new BufferedOutputStream(fstream); 
           stream.write(b);
         
        } catch (Exception e) { 
	//	            log.error("helper:get file from byte process error!"); 
	    e.printStackTrace(); 
	} finally {
	    if (stream != null) {
	        try {
	            stream.close(); 
	        } catch (IOException e) {
	//	                    log.error("helper:get file from byte process error!"); 
	                e.printStackTrace(); 
	            }
	        }
	    }
	    return ret; 
	}
	 
	/**
	  * 获取现在时间
	  * 
	  * @return返回字符串格式 yyyy-MM-dd HH:mm:ss
	  */
	@SuppressLint("SimpleDateFormat")
	public static String getStringDate() {
	  Date currentTime = new Date(System.currentTimeMillis());
	  SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
	  String dateString = formatter.format(currentTime);
	  return dateString;
	}
	
	/** 
     * 读取文件的内容 
     * @param filename 文件名称 
     * @return 
     * @throws Exception 
     */  
    public String readFile(Context context,String filename) throws Exception  
    {  
        //获得输入流  
        FileInputStream inStream = context.openFileInput(filename);  
        //new一个缓冲区  
        byte[] buffer = new byte[1024];  
        int len = 0;  
        //使用ByteArrayOutputStream类来处理输出流  
        ByteArrayOutputStream outStream = new ByteArrayOutputStream();  
        while( (len = inStream.read(buffer))!= -1)  
        {  
            //写入数据  
            outStream.write(buffer, 0, len);  
        }  
        //得到文件的二进制数据  
        byte[] data = outStream.toByteArray();  
        //关闭流  
        outStream.close();  
        inStream.close();  
        return new String(data);  
    }
    
	public static void saveBitmap(Bitmap bm) {
			File f = new File("/sdcard/test");
			if (!f.exists()) {
				f.mkdir();
			}
		  f = new File("/sdcard/test/", "test"+ curImageIndex +".jpg");
		  curImageIndex++;

		  try {
		   FileOutputStream out = new FileOutputStream(f);
		   bm.compress(Bitmap.CompressFormat.JPEG, 100, out);
		   out.flush();
		   out.close();
		  } catch (FileNotFoundException e) {
		   // TODO Auto-generated catch block
		   e.printStackTrace();
		  } catch (IOException e) {
		   // TODO Auto-generated catch block
		   e.printStackTrace();
		  }
		 }
}
