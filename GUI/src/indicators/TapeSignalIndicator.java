package indicators;

import javafx.geometry.Pos;
import javafx.scene.control.Label;
import javafx.scene.control.Toggle;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

/**
 * A GUI control that displays the actions the robot takes depending on tapes.
 * @author Emil Berg
 *
 */
public class TapeSignalIndicator extends SignalIndicatorParent {

	private final Label tapeLabel;
	
	private final ToggleGroup tapeGroup;
	
	private final ToggleButton tapeLeft;
	private final ToggleButton tapeStart;
	private final ToggleButton tapeRight;
	private final ToggleButton tapeFinish;

	public TapeSignalIndicator() {

		this.setAlignment(Pos.CENTER);

		VBox armMainBox = new VBox();
		VBox sigMainBox = new VBox();
		VBox tapeMainBox = new VBox();
		
		this.tapeLabel = new Label("Tape signals");
		
		HBox tapeBox1 = new HBox();
		HBox tapeBox2 = new HBox();

		tapeMainBox.setAlignment(Pos.CENTER);
		
		tapeBox1.setAlignment(Pos.TOP_CENTER);
		tapeBox2.setAlignment(Pos.BOTTOM_CENTER);

		this.tapeGroup = new ToggleGroup();
		
		this.tapeLeft = generateButton("↰", tapeGroup);
		this.tapeStart = generateButton("START", tapeGroup);
		this.tapeRight = generateButton("↱", tapeGroup);
		this.tapeFinish = generateButton("FINISH", tapeGroup);

		tapeBox1.getChildren().addAll(this.tapeStart);
		tapeBox2.getChildren().addAll(this.tapeLeft, this.tapeFinish, this.tapeRight);
		
		tapeMainBox.getChildren().addAll(this.tapeLabel, tapeBox1, tapeBox2);

		this.getChildren().addAll(armMainBox, sigMainBox, tapeMainBox);
	}
	
	public void clear() {
		Toggle sel = this.tapeGroup.getSelectedToggle();
		if (sel != null) {
			sel.setSelected(false);
		}
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
