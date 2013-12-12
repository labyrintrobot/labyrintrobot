package indicators;

import javafx.geometry.Pos;
import javafx.scene.control.Label;
import javafx.scene.control.Toggle;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

/**
 * A GUI control that displays the actions of the robot's wheel engines.
 * @author Emil Berg
 *
 */
public class ControlSignalIndicator extends SignalIndicatorParent {

	private final Label sigLabel;
	
	private final ToggleGroup dirSignalsGroup;
	
	private final ToggleButton leftForward;
	private final ToggleButton forward;
	private final ToggleButton rightForward;
	private final ToggleButton rotateLeft;
	private final ToggleButton stop;
	private final ToggleButton rotateRight;
	private final ToggleButton backwards;

	public ControlSignalIndicator() {

		this.setAlignment(Pos.CENTER);

		VBox sigMainBox = new VBox();
		
		this.sigLabel = new Label("Control signals");
		
		HBox sigBox1 = new HBox();
		HBox sigBox2 = new HBox();
		HBox sigBox3 = new HBox();

		sigMainBox.setAlignment(Pos.CENTER);
		
		sigBox1.setAlignment(Pos.TOP_CENTER);
		sigBox2.setAlignment(Pos.CENTER);
		sigBox3.setAlignment(Pos.BOTTOM_CENTER);
		
		this.dirSignalsGroup = new ToggleGroup();
		this.leftForward = generateButton("↖", dirSignalsGroup);
		this.forward = generateButton("↑", dirSignalsGroup);
		this.rightForward = generateButton("↗", dirSignalsGroup);
		this.rotateLeft = generateButton("↺", dirSignalsGroup);
		this.stop = generateButton("↥", dirSignalsGroup);
		this.rotateRight = generateButton("↻", dirSignalsGroup);
		this.backwards = generateButton("↓", dirSignalsGroup);
		
		sigBox1.getChildren().addAll(this.leftForward, this.forward, this.rightForward);
		sigBox2.getChildren().addAll(this.rotateLeft, this.stop, this.rotateRight);
		sigBox3.getChildren().addAll(this.backwards);
		
		this.getChildren().addAll(this.sigLabel, sigBox1, sigBox2, sigBox3);
	}
	
	public void clear() {
		Toggle sel = this.dirSignalsGroup.getSelectedToggle();
		if (sel != null) {
			sel.setSelected(false);
		}
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
}
