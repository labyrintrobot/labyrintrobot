package application;

import javafx.geometry.Pos;
import javafx.scene.control.Label;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

/**
 * A GUI control that displays to the user what the control module is doing.
 * @author Emil Berg
 *
 */
public class ControlPad extends VBox {

	private final Label armLabel;
	private final Label sigLabel;
	private final Label tapeLabel;
	
	private final ToggleGroup armSignalsGroup;
	private final ToggleGroup dirSignalsGroup;
	private final ToggleGroup tapeGroup;

	private final ToggleButton openArm;
	private final ToggleButton closeArm;
	
	private final ToggleButton leftForward;
	private final ToggleButton forward;
	private final ToggleButton rightForward;
	private final ToggleButton rotateLeft;
	private final ToggleButton stop;
	private final ToggleButton rotateRight;
	private final ToggleButton backwards;
	
	private final ToggleButton tapeLeft;
	private final ToggleButton tapeStart;
	private final ToggleButton tapeRight;
	private final ToggleButton tapeFinish;

	public ControlPad() {

		this.setAlignment(Pos.CENTER);
		this.setSpacing(10);

		VBox armMainBox = new VBox();
		VBox sigMainBox = new VBox();
		VBox tapeMainBox = new VBox();
		
		this.armLabel = new Label("Arm signals");
		this.sigLabel = new Label("Control signals");
		this.tapeLabel = new Label("Tape signals");
		
		HBox armBox = new HBox();
		HBox sigBox1 = new HBox();
		HBox sigBox2 = new HBox();
		HBox sigBox3 = new HBox();
		HBox tapeBox1 = new HBox();
		HBox tapeBox2 = new HBox();

		armMainBox.setAlignment(Pos.CENTER);
		sigMainBox.setAlignment(Pos.CENTER);
		tapeMainBox.setAlignment(Pos.CENTER);
		
		armBox.setAlignment(Pos.CENTER);
		
		sigBox1.setAlignment(Pos.TOP_CENTER);
		sigBox2.setAlignment(Pos.CENTER);
		sigBox3.setAlignment(Pos.BOTTOM_CENTER);
		
		tapeBox1.setAlignment(Pos.TOP_CENTER);
		tapeBox2.setAlignment(Pos.BOTTOM_CENTER);

		this.dirSignalsGroup = new ToggleGroup();
		this.armSignalsGroup = new ToggleGroup();
		this.tapeGroup = new ToggleGroup();
		
		this.openArm = generateButton("V", armSignalsGroup);
		this.closeArm = generateButton("||", armSignalsGroup);
		
		this.leftForward = generateButton("↖", dirSignalsGroup);
		this.forward = generateButton("↑", dirSignalsGroup);
		this.rightForward = generateButton("↗", dirSignalsGroup);
		this.rotateLeft = generateButton("↺", dirSignalsGroup);
		this.stop = generateButton("↥", dirSignalsGroup);
		this.rotateRight = generateButton("↻", dirSignalsGroup);
		this.backwards = generateButton("↓", dirSignalsGroup);
		
		this.tapeLeft = generateButton("↰", tapeGroup);
		this.tapeStart = generateButton("START", tapeGroup);
		this.tapeRight = generateButton("↱", tapeGroup);
		this.tapeFinish = generateButton("FINISH", tapeGroup);

		armBox.getChildren().addAll(this.openArm, this.closeArm);
		sigBox1.getChildren().addAll(this.leftForward, this.forward, this.rightForward);
		sigBox2.getChildren().addAll(this.rotateLeft, this.stop, this.rotateRight);
		sigBox3.getChildren().addAll(this.backwards);
		tapeBox1.getChildren().addAll(this.tapeStart);
		tapeBox2.getChildren().addAll(this.tapeLeft, this.tapeFinish, this.tapeRight);
		
		armMainBox.getChildren().addAll(this.armLabel, armBox);
		sigMainBox.getChildren().addAll(this.sigLabel, sigBox1, sigBox2, sigBox3);
		tapeMainBox.getChildren().addAll(this.tapeLabel, tapeBox1, tapeBox2);

		this.getChildren().addAll(armMainBox, sigMainBox, tapeMainBox);
	}

	private ToggleButton generateButton(String text, ToggleGroup group) {
		ToggleButton b = new ToggleButton(text);
		b.setToggleGroup(group);
		b.setDisable(true);
		b.setStyle("-fx-font: 18 arial;");
		
		setFocusTraversable(false);
		
		return b;
	}

	public void pressOpenArm() {
		this.openArm.setSelected(true);
	}
	public void pressCloseArm() {
		this.closeArm.setSelected(true);
	}
	
	public void pressLeftForward() {
		this.leftForward.setSelected(true);
	}
	public void pressForward() {
		this.forward.setSelected(true);
	}
	public void pressRightForward() {
		this.rightForward.setSelected(true);
	}
	public void pressRotateLeft() {
		this.rotateLeft.setSelected(true);
	}
	public void pressStop() {
		this.stop.setSelected(true);
	}
	public void pressRotateRight() {
		this.rotateRight.setSelected(true);
	}
	public void pressBackwards() {
		this.backwards.setSelected(true);
	}
	
	public void pressTapeLeft() {
		this.tapeLeft.setSelected(true);
	}
	public void pressTapeStart() {
		this.tapeStart.setSelected(true);
	}
	public void pressTapeRight() {
		this.tapeRight.setSelected(true);
	}
	public void pressTapeFinish() {
		this.tapeFinish.setSelected(true);
	}
}
