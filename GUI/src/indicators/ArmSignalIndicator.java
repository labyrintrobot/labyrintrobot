package indicators;

import javafx.geometry.Pos;
import javafx.scene.control.Label;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.layout.HBox;

/**
 * A GUI control that displays to the user what the control module is doing.
 * @author Emil Berg
 *
 */
public class ArmSignalIndicator extends SignalIndicatorParent {

	private final Label armLabel;
	
	private final ToggleGroup armSignalsGroup;

	private final ToggleButton openArm;
	private final ToggleButton closeArm;

	public ArmSignalIndicator() {

		this.setAlignment(Pos.CENTER);
		
		this.armLabel = new Label("Arm signals");
		
		HBox armBox = new HBox();
		
		armBox.setAlignment(Pos.CENTER);

		this.armSignalsGroup = new ToggleGroup();
		
		this.openArm = generateButton("V", armSignalsGroup);
		this.closeArm = generateButton("||", armSignalsGroup);
		
		armBox.getChildren().addAll(this.openArm, this.closeArm);
		
		this.getChildren().addAll(this.armLabel, armBox);
	}

	public void pressOpenArm() {
		this.openArm.setSelected(true);
	}
	public void pressCloseArm() {
		this.closeArm.setSelected(true);
	}
}
