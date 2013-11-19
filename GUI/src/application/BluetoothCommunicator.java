package application;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import javax.microedition.io.Connector;
import javax.obex.ClientSession;
import javax.obex.HeaderSet;
import javax.obex.Operation;
import javax.obex.ResponseCodes;

public class BluetoothCommunicator {

	private boolean isSetup = false;
	
	private ClientSession clientSession;
	private Operation putOperation;
	private OutputStream os;
	private InputStream is;

	public void setup(String serverURL) throws IOException {
		
		if (isSetup) {
			throw new IllegalStateException("Already set up");
		}
		isSetup = true;
		
		System.out.println("Connecting to " + serverURL);

		clientSession = (ClientSession) Connector.open(serverURL);
		HeaderSet hsConnectReply = clientSession.connect(null);
		if (hsConnectReply.getResponseCode() != ResponseCodes.OBEX_HTTP_OK) {
			System.out.println("Failed to connect");
			return;
		}

		HeaderSet hsOperation = clientSession.createHeaderSet();
		hsOperation.setHeader(HeaderSet.NAME, "Hello.txt");
		hsOperation.setHeader(HeaderSet.TYPE, "text");

		// Create PUT Operation
		putOperation = clientSession.put(hsOperation);
		os = putOperation.openOutputStream();
		is = putOperation.openInputStream();
	}

	public void teardown() throws IOException {
		
		if (! isSetup) {
			throw new IllegalStateException("Has not been set up");
		}
		isSetup = false;
		
		os.close();
		is.close();

		putOperation.close();

		clientSession.disconnect(null);

		clientSession.close();
	}

	public byte[] receiveMessage(int length) throws IOException {
		if (! isSetup) {
			throw new IllegalStateException("Has not been set up");
		}
		
		byte[] ret = new byte[length];
		is.read(ret);
		return ret;
	}

	public void sendMessageToDevice(byte[] message) throws IOException {
		if (! isSetup) {
			throw new IllegalStateException("Has not been set up");
		}
		
		// Send some text to server
		os.write(message);
	}
}
