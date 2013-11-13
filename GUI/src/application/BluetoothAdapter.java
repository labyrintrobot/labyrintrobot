package application;

public class BluetoothAdapter {
	
	// TODO: Concurrency
	
	private final IMessageReceiver receiver;
	
	public BluetoothAdapter(IMessageReceiver receiver) {
		this.receiver = receiver;
	}
	
	public interface IMessageReceiver {
		public void receiveMessage(byte header, byte data);
	}
	
	public void sendMessage(byte header, byte data) {
		// TODO
		// TODO CRC?
	}
	
	public void receiveMessages() {
		// TODO
		//receiver.receiveMessage(header, data);
	}
	
	public void exit() {
		// TODO
	}
}
