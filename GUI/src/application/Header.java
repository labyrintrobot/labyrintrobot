package application;

/**
 * The different types of headers used for communication.
 * @author Emil Berg
 * 
 */
public enum Header {
	CONTROL_COMMAND(0x00),
	CONTROL_SIGNAL(0x01),
	CALIBRATION(0x02),
	DISTANCE_LEFT_SHORT(0x03),
	DISTANCE_LEFT_LONG(0x04),
	DISTANCE_FORWARD_LEFT(0x05),
	DISTANCE_FORWARD_CENTER(0x06),
	DISTANCE_FORWARD_RIGHT(0x07),
	DISTANCE_RIGHT_LONG(0x08),
	DISTANCE_RIGHT_SHORT(0x09),
	CONTROL_ERROR(0x0A),
	TAPE(0x0B),
	ERROR(0x0C),
	PING(0x0D),
	P_MSB(0x0E),
	P_LSB(0x0F),
	D_MSB(0x10),
	D_LSB(0x11),
	SPEED(0x12);

	private final int i;

	private Header(int i) {
		this.i = i;
	}

	public int getIndex() {
		return i;
	}

	public static Header fromInt(int i) {
		for (Header s : Header.values()) {
			if (s.i == i) {
				return s;
			}
		}
		return null;
	}
}
