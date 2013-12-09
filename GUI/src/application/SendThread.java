package application;

/**
 * A simple thread that periodically sends messages to the robot.
 * @author Emil Berg
 *
 */
public class SendThread implements Runnable {

	private final BluetoothAdapter bluetoothAdapter;
	private final NumericUpDown pn;
	private final NumericUpDown dn;
	private final NumericUpDown sn;
	
	public SendThread(BluetoothAdapter ba, NumericUpDown pn, NumericUpDown dn, NumericUpDown sn) {
		this.bluetoothAdapter = ba;
		this.pn = pn;
		this.dn = dn;
		this.sn = sn;
	}
	
	@Override
	public void run() {
		while (true) {
			try {
				Thread.sleep(1000);
				bluetoothAdapter.sendMessage(Header.P_LSB, pn.getVal());
				Thread.sleep(1000);
				bluetoothAdapter.sendMessage(Header.D_LSB, dn.getVal());
				Thread.sleep(1000);
				bluetoothAdapter.sendMessage(Header.SPEED, sn.getVal());
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
}
