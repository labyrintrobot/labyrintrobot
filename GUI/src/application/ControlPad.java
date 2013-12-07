package application;

import javafx.geometry.Pos;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

/**
 * A GUI control that displays what the control module is doing to the user.
 * @author Emil Berg
 *
 */
public class ControlPad extends VBox {

	final ToggleGroup group;

	private final ToggleButton openArm;
	private final ToggleButton closeArm;
	private final ToggleButton leftForward;
	private final ToggleButton forward;
	private final ToggleButton rightForward;
	private final ToggleButton rotateLeft;
	private final ToggleButton stop;
	private final ToggleButton rotateRight;
	private final ToggleButton backwards;

	public ControlPad() {

		this.setAlignment(Pos.CENTER);

		HBox r1 = new HBox();
		HBox r2 = new HBox();
		HBox r3 = new HBox();
		HBox r4 = new HBox();

		r1.setAlignment(Pos.TOP_CENTER);
		r2.setAlignment(Pos.CENTER);
		r3.setAlignment(Pos.CENTER);
		r4.setAlignment(Pos.BOTTOM_CENTER);

		this.group = new ToggleGroup();
		this.openArm = generateButton("V");
		this.closeArm = generateButton("||");
		this.leftForward = generateButton("↖");
		this.forward = generateButton("↑");
		this.rightForward = generateButton("↗");
		this.rotateLeft = generateButton("↺");
		this.stop = generateButton("↥");
		this.rotateRight = generateButton("↻");
		this.backwards = generateButton("↓");

		r1.getChildren().addAll(this.openArm, this.closeArm);
		r2.getChildren().addAll(this.leftForward, this.forward, this.rightForward);
		r3.getChildren().addAll(this.rotateLeft, this.stop, this.rotateRight);
		r4.getChildren().addAll(this.backwards);

		this.getChildren().addAll(r1, r2, r3, r4);
		
		this.stop.setSelected(true);
	}

	private ToggleButton generateButton(String text) {
		ToggleButton b = new ToggleButton(text);
		b.setToggleGroup(group);
		b.setDisable(true);
		b.setStyle("-fx-font: 22 arial;");
		
		setFocusTraversable(false);
		
		return b;
	}

	public void pressLeftForward() {
		leftForward.setSelected(true);
	}
	public void pressForward() {
		forward.setSelected(true);
	}
	public void pressRightForward() {
		rightForward.setSelected(true);
	}
	public void pressRotateLeft() {
		rotateLeft.setSelected(true);
	}
	public void pressStop() {
		stop.setSelected(true);
	}
	public void pressRotateRight() {
		rotateRight.setSelected(true);
	}
	public void pressBackwards() {
		backwards.setSelected(true);
	}
	public void pressOpenArm() {
		openArm.setSelected(true);
	}
	public void pressCloseArm() {
		closeArm.setSelected(true);
	}
}
