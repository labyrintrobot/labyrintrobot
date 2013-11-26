package application;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;

import javax.bluetooth.ServiceRecord;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;

public class BluetoothCommunicator {

	private boolean isSetup = false;
	
	private StreamConnection conn;
	private OutputStream os;
	private InputStream is;

	public void setup(ServiceRecord bluetoothService) throws IOException {
		
		if (isSetup) {
			throw new IllegalStateException("Already set up");
		}
		isSetup = true;
		
		String url = bluetoothService.getConnectionURL(ServiceRecord.AUTHENTICATE_NOENCRYPT, false);

		try {
			conn = (StreamConnection) Connector.open(url);
			os = conn.openOutputStream();
			is = conn.openDataInputStream();

		} catch (Exception e) {
			System.out.println("Failed to connect to " + url);
			e.printStackTrace();
		}
	}

	public void teardown() throws IOException {
		
		if (! isSetup) {
			throw new IllegalStateException("Has not been set up");
		}
		isSetup = false;
		
		os.close();
		is.close();
		
		conn.close();
	}

	public int[] receiveMessage(int length) throws IOException {
		if (! isSetup) {
			throw new IllegalStateException("Has not been set up");
		}
		
		int[] ret = new int[length];
		for (int i = 0; i < length; i++) {
			ret[i] = is.read();
		}
		System.out.println("Received: " + Arrays.toString(ret));
		
		return ret;
	}

	public void sendMessageToDevice(int[] message) throws IOException {
		if (! isSetup) {
			throw new IllegalStateException("Has not been set up");
		}
		
		System.out.println("Sent: " + Arrays.toString(message));
		for (int i = 0; i < message.length; i++) {
			os.write(message[i]);
		}
	}
	
	/*public void broadcastCommand(String str) {
		String url = this.service.getConnectionURL(ServiceRecord.AUTHENTICATE_NOENCRYPT, false);

		StreamConnection conn = null;

		try {
			System.out.println("Sending command to " + url);

			conn = (StreamConnection) Connector.open(url);
			DataInputStream din = conn.openDataInputStream();

			byte[] b = new byte[2];
			for (int i = 0; i < 20; i++) {
				int s = din.read(b);
				System.out.println(b[0]);
				System.out.println(b[1]);
			}

			din.close();
			conn.close();

			System.out.println("Sent. Connection Closed.");

		} catch (Exception e) {
			System.out.println("Failed to connect to " + url);
			e.printStackTrace();
		}
	}*/
}
