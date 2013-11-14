package application;

import javafx.geometry.Pos;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

public class ControlPad extends VBox {

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

		r1.setAlignment(Pos.TOP_CENTER);
		r2.setAlignment(Pos.CENTER);
		r3.setAlignment(Pos.BOTTOM_CENTER);

		leftForward = generateButton("↖");
		forward = generateButton("↑");
		rightForward = generateButton("↗");
		rotateLeft = generateButton("↺");
		stop = generateButton("⇞");
		rotateRight = generateButton("↻");
		backwards = generateButton("↓");
		
		ToggleGroup group = new ToggleGroup();
		leftForward.setToggleGroup(group);
		forward.setToggleGroup(group);
		rightForward.setToggleGroup(group);
		rotateLeft.setToggleGroup(group);
		stop.setToggleGroup(group);
		rotateRight.setToggleGroup(group);
		backwards.setToggleGroup(group);
		group.
		
		r1.getChildren().addAll(leftForward, forward, rightForward);
		r2.getChildren().addAll(rotateLeft, stop, rotateRight);
		r3.getChildren().addAll(backwards);

		this.getChildren().addAll(r1, r2, r3);
	}

	private ToggleButton generateButton(String text) {
		ToggleButton b = new ToggleButton(text);
		//b.setDisable(true);
		b.setStyle("-fx-font: 26 arial;");
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
}
