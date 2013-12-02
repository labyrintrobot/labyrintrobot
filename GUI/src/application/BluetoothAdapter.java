package application;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.Random;

import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;
import javafx.application.Platform;

public class BluetoothAdapter {

	private static final boolean DEBUG = false;

	private final String bluetoothUrl;
	private final IMessageReceiver receiver;
	private final Random rng = new Random();

	private boolean isSetup = false;
	private boolean running = true;
	private StreamConnection conn;
	private DataOutputStream os;
	private InputStream is;

	public BluetoothAdapter(String bluetoothUrl, IMessageReceiver receiver) {
		this.bluetoothUrl = bluetoothUrl;
		this.receiver = receiver;
	}

	private void setup() throws IOException {
		if (! DEBUG) {
			conn = (StreamConnection) Connector.open(this.bluetoothUrl);
			os = new DataOutputStream(conn.openOutputStream());
			is = conn.openDataInputStream();
		}
		isSetup = true;
		receiver.communicationGained();
	}

	public interface IMessageReceiver {
		public void receiveMessage(int header, int data);
		public void communicationGained();
		public void communicationLost();
	}

	public void sendMessage(int header, int data) {
		if (! DEBUG) {
			int[] b = new int[2];
			b[0] = header;
			b[1] = data;
			if (isSetup) {
				try {
					sendMessageToDevice(b);
				} catch (IOException e) {
					this.teardown();
					Platform.runLater(new Runnable() {

						@Override
						public void run() {
							receiver.communicationLost();
						}
					});
				}
			}
		}
	}

	public void receiveMessages() {
		while (running) {
			if (DEBUG) {
				try {
					Thread.sleep(10);
				} catch (InterruptedException e) {
					// Do nothing
				}

				final byte header = (byte)rng.nextInt(0x0C);
				final int data;
				if (header == 0x01) {
					data = rng.nextInt(0x07);
				} else {
					data = rng.nextInt(0xFF);
				}

				Platform.runLater(new Runnable() {

					@Override
					public void run() {
						receiver.receiveMessage(header, data);
					}
				});
			} else {
				if (isSetup) {
					try {
						final int[] ret = receiveMessage(2);

						Platform.runLater(new Runnable() {

							@Override
							public void run() {
								receiver.receiveMessage(ret[0], ret[1]);
							}
						});
					} catch (IOException e) {
						this.teardown();
						Platform.runLater(new Runnable() {

							@Override
							public void run() {
								receiver.communicationLost();
							}
						});
					}
				} else {
					try {
						this.setup();
					} catch (IOException e) {
						this.teardown();
					}
				}

			}
		}
		this.teardown();
	}

	public void exit() {
		running = false;
	}

	private void teardown() {
		if (isSetup) {
			try {
				os.close();
				is.close();
				conn.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		this.isSetup = false;
	}

	private synchronized int[] receiveMessage(int length) throws IOException {

		int[] ret = new int[length];
		for (int i = 0; i < length; i++) {
			ret[i] = is.read();
			if (ret[i] == -1) {
				throw new IOException();
			}
		}
		System.out.println("Received: " + Arrays.toString(ret));

		return ret;
	}

	private synchronized void sendMessageToDevice(int[] message) throws IOException {
		System.out.println("Sent: " + Arrays.toString(message));
		for (int i = 0; i < message.length; i++) {
			os.write(message[i]);
		}
		os.flush();
	}
}
