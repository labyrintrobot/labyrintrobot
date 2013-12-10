package indicators;

import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.layout.VBox;

/**
 * A GUI control that displays to the user what the control module is doing.
 * @author Emil Berg
 *
 */
public abstract class SignalIndicatorParent extends VBox {

	protected ToggleButton generateButton(String text, ToggleGroup group) {
		ToggleButton b = new ToggleButton(text);
		b.setToggleGroup(group);
		b.setDisable(true);
		b.setStyle("-fx-font: 18 arial;");
		
		setFocusTraversable(false);
		
		return b;
	}
}
