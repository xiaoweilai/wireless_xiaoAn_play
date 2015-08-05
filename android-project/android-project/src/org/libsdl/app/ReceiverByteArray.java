package org.libsdl.app;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.nio.ByteBuffer;

import org.libsdl.app.SDLActivity.MyHandler;

import android.content.Context;
import android.graphics.Bitmap;
import android.os.Message;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class ReceiverByteArray {
	private byte[] mCopyByteMessage;
	private byte[] mGetByteMessage;
	private byte[] mGetImageNameByte;
	private int mByteLong;
	private int mCurImageSize = 0;
	private int mCurImageNameLength = 0;
	Socket mSocket;
	private static boolean mReceiveSuccessedFlag = false;
	private static boolean mReceiveSuccessedBeforeFourByteFlag = false;
	Bitmap mImgBitmapShow;
	Message msg;

	private RelativeLayout mRelMain;
	private MyAPP mAPP = null; 
	private Thread receiveImag;
	private String mIpAddress;
	
	long decoder;
	private int screenWidth = 1920;
	private int screenHeight = 1080;
	byte[] mPixel = new byte[screenWidth * screenHeight*4];
	ByteBuffer buffer = ByteBuffer.wrap(mPixel);
	private long packageHead = 0x00000000FFFEFFFEL;
	byte bytes = 0x40;
	private static String curStartTimeFileName;
	
	//sdl add
	private Bitmap mBitmap;
	private int mSecs = 0;
	private int ulCount = 0;
	private Context mContext;
	private MyHandler mHandler;
	
	public ReceiverByteArray(Context context, MyHandler handler){
		mContext = context;
		mHandler = handler;
		
	}

	public void startServer(){
		try{
			SocketServer();
		}catch(Throwable e){
			e.printStackTrace();
			PublicFunction.writeFile(mContext, getLogFileName(), "SocketServer Th rowable" + Thread.currentThread().getName());
			System.out.println("SocketServer Th rowable");
			mHandler.sendEmptyMessage(6);
			if(receiveImag != null){
				receiveImag.interrupt();
				receiveImag = null;
			}
				mReceiveSuccessedFlag = true;
		} finally{
			
		}	
	}
	
	public static String getLogFileName(){
		return curStartTimeFileName + ".txt";
	}
	
	private void SocketServer(){
		mByteLong = 0;
		mCurImageSize = 0;
		mCurImageNameLength = 0;
		mHandler.sendEmptyMessage(5);
		mReceiveSuccessedFlag = false;
		receiveImag = new Thread(run);
		receiveImag.start();
	}
	
	Runnable run = new Runnable() {
		ServerSocket ss = null;
		DataOutputStream mDos = null;
		DataInputStream mDis = null;
		@Override
		public void run() {
	         try {
	        	 mHandler.sendEmptyMessage(2);
				 ss = new ServerSocket();
	        	 ss.bind(new InetSocketAddress(mIpAddress,16689)); 

		         //服务器接收到客户端的数据后，创建与此客户端对话的Socket
	        	 mSocket = ss.accept();
	        	 mHandler.sendEmptyMessage(3);
		         //用于向客户端发送数据的输出流
		         mDos = new DataOutputStream(mSocket.getOutputStream());
		         //用于接收客户端发来的数据的输入流
		         mDis = new DataInputStream(mSocket.getInputStream());
		         mHandler.sendEmptyMessageDelayed(8, 5000);
		         while(!mReceiveSuccessedFlag){
		        	 receiveMessage(mDis,mSocket,mDos);
	        	 }
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				if(!mReceiveSuccessedFlag){
					PublicFunction.writeFile(mContext, getLogFileName(), "线程连接异常   run " + Thread.currentThread().getName());
					System.out.println("线程连接异常   run " + Thread.currentThread().getName());
					mHandler.sendEmptyMessage(6);
					if(receiveImag != null){
						receiveImag.interrupt();
					}
				}
				mReceiveSuccessedFlag = true;
				receiveImag = null;
				try {
     				if (null != mDis)
     					mDis.close();
     				if (null != mDos)
     					mDos.close();
     				if (null != ss){
     					ss.close();
     				}
     			} catch (IOException ee) {
     				ee.printStackTrace();
     			}
			}
			
		}
	};

public void receiveMessage(DataInputStream input, Socket s, DataOutputStream output) throws SocketException{
		
		int numRead = 0;
		try {
			int tempBytelength = input.available();
			if(!mReceiveSuccessedBeforeFourByteFlag){
	            if(tempBytelength >= 100){
	            	long tempTotalSize = input.readLong();
	            	long readPackageHead = input.readLong();
	            	long readPackageContent = input.readLong();
	            	if(readPackageHead != 8 || readPackageContent != packageHead){
	            		return;
	            	}
	            	mCurImageSize = (int)tempTotalSize - 24;
	            	mGetByteMessage = new byte[mCurImageSize];
	            	mReceiveSuccessedBeforeFourByteFlag = true;
	            }
	            return;
			}else {
					numRead=input.read(mGetByteMessage, mByteLong, mCurImageSize - mByteLong);
					if(mByteLong < mCurImageSize){
						mByteLong += numRead;
						return;
					} else if(mByteLong >= mCurImageSize){
//						PublicFunction.writeFile(mContext, "savefilelog.txt", "TotalBytes:" + tempTotalSize +"\n");
//						PublicFunction.writeFile(mContext, "savefilelog.txt", "TotalBytes:" + tempTotalSize);
//						PublicFunction.writeFile(mContext, System.currentTimeMillis()+"file.txt", mGetByteMessage);
						mByteLong = 0;
						mCurImageSize = 0;
						mCurImageNameLength = 0;
						tempBytelength = 0;
						msg = new Message();
						msg.what = 1;
						mHandler.sendMessage(msg);
						mReceiveSuccessedBeforeFourByteFlag = false;
						
					}
			}
		} catch (IOException e) {
			PublicFunction.writeFile(mContext, getLogFileName(), "socket input output异常" + Thread.currentThread().getName());
			PublicFunction.writeFile(mContext, getLogFileName(), "线程连接异常" + Thread.currentThread().getName());
			System.out.println("线程连接异常  receiveMessage" + Thread.currentThread().getName());
			try {
				if (null != input)
					input.close();
				if (null != output)
					output.close();
				if (null != s)
					s.close();
			} catch (IOException ee) {
				ee.printStackTrace();
			}
			mHandler.sendEmptyMessage(6);
			mReceiveSuccessedFlag = true;
			if(receiveImag != null){
				receiveImag.interrupt();
				receiveImag = null;
			}
		} 
	}
}
