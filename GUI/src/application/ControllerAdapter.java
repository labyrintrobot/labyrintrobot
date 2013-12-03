package application;

import java.io.IOException;

public class ControllerAdapter {
	private boolean rightPressed = false;
	private boolean upPressed = false;
	private boolean leftPressed = false;
	private boolean downPressed = false;
	
	private boolean openArm = true;

	private final BluetoothAdapter bluetoothAdapter;

	public ControllerAdapter(BluetoothAdapter bluetoothAdapter) {
		this.bluetoothAdapter = bluetoothAdapter;
	}

	public void pressRight() throws IOException {
		if (rightPressed == false) {
			rightPressed = true;
			revalidateButtons();
		}
	}

	public void releaseRight() throws IOException {
		if (rightPressed) {
			rightPressed = false;
			revalidateButtons();
		}
	}

	public void pressUp() throws IOException {
		if (upPressed == false) {
			upPressed = true;
			revalidateButtons();
		}
	}


	public void releaseUp() throws IOException {
		if (upPressed) {
			upPressed = false;
			revalidateButtons();
		}
	}

	public void pressLeft() throws IOException {
		if (leftPressed == false) {
			leftPressed = true;
			revalidateButtons();
		}
	}

	public void releaseLeft() throws IOException {
		if (leftPressed) {
			leftPressed = false;
			revalidateButtons();
		}
	}

	public void pressDown() throws IOException {
		if (downPressed == false) {
			downPressed = true;
			revalidateButtons();
		}
	}

	public void releaseDown() throws IOException {
		if (downPressed) {
			downPressed = false;
			revalidateButtons();
		}
	}

	public void pressC() throws IOException {
		final byte header = 0x02;
		final byte data = 0x00;

		bluetoothAdapter.sendMessage(header, data);
	}

	public void releaseC() {
		// Do nothing
	}
	
	public void pressQ() {
		final byte header = 0x00;
		final byte data;
		if (openArm) {
			System.out.println("Open arm");
			data = 0x09;
		} else {
			System.out.println("Close arm");
			data = 0x0A;
		}
		bluetoothAdapter.sendMessage(header, data);
		openArm = !openArm;
	}

	public void releaseQ() {
		// Do nothing
	}
	
	public void pressW() {
		final byte header = 0x00;
		final byte data = 0x05;
		bluetoothAdapter.sendMessage(header, data);
	}

	public void releaseW() {
		// Do nothing
	}
	
	public void pressE() {
		final byte header = 0x00;
		final byte data = 0x04;
		bluetoothAdapter.sendMessage(header, data);
	}

	public void releaseE() {
		// Do nothing
	}

	private void revalidateButtons() throws IOException {
		final byte header = 0x00;
		final byte data;

		if (upPressed) {
			if (rightPressed == leftPressed) {
				System.out.println("up");
				data = 0x00;
			} else {
				if (rightPressed) {
					System.out.println("right");
					data = 0x02;
				} else {
					System.out.println("left");
					data = 0x03;
				}
			}
		} else {
			if (downPressed) {
				System.out.println("back");
				data = 0x01;
			} else if (rightPressed != leftPressed) {
				if (rightPressed) {
					System.out.println("rotate right");
					data = 0x07;
				} else {
					System.out.println("rotate left");
					data = 0x08;
				}
			} else {
				System.out.println("stop");
				data = 0x06;
			}
		}

		bluetoothAdapter.sendMessage(header, data);
	}
}
