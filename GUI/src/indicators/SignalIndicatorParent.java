package indicators;

import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.layout.VBox;

/**
 * Parent for a GUI control that displays to the user what the robot is doing.
 * @author Emil Berg
 *
 */
public abstract class SignalIndicatorParent extends VBox {

	public abstract void clear();
	
	protected ToggleButton generateButton(String text, ToggleGroup group) {
		ToggleButton b = new ToggleButton(text);
		b.setToggleGroup(group);
		b.setDisable(true);
		b.setStyle("-fx-font: 18 arial;");
		
		setFocusTraversable(false);
		
		return b;
	}
}
