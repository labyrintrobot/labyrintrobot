package application;

import java.util.HashMap;
import java.util.Map;

import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.geometry.Pos;
import javafx.scene.control.Toggle;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

/**
 * A GUI control that allows the user to select which LineChart to display.
 * @author Emil Berg
 *
 */
public class ChartSelectorControl extends VBox {
	
	public interface ToggleCallback {
		public void callback();
	}
	
	private final Map<Toggle, Diagram> map;

	final ToggleGroup group;

	private final ToggleButton distanceLeftShort;
	private final ToggleButton distanceLeftLong;
	private final ToggleButton distanceForwardLeft;
	private final ToggleButton distanceForwardCenter;
	private final ToggleButton distanceForwardRight;
	private final ToggleButton distanceRightLong;
	private final ToggleButton distanceRightShort;
	private final ToggleButton tape;
	private final ToggleButton controlError;

	public ChartSelectorControl(final ToggleCallback callback) {

		this.setAlignment(Pos.CENTER);

		HBox r1 = new HBox();
		HBox r2 = new HBox();
		HBox r3 = new HBox();

		r1.setAlignment(Pos.TOP_CENTER);
		r2.setAlignment(Pos.CENTER);
		r3.setAlignment(Pos.BOTTOM_CENTER);

		distanceLeftShort = generateButton(Diagram.DISTANCE_LEFT_SHORT.getButtonText());
		distanceLeftLong = generateButton(Diagram.DISTANCE_LEFT_LONG.getButtonText());
		distanceForwardLeft = generateButton(Diagram.DISTANCE_FORWARD_LEFT.getButtonText());
		distanceForwardCenter = generateButton(Diagram.DISTANCE_FORWARD_CENTER.getButtonText());
		distanceForwardRight = generateButton(Diagram.DISTANCE_FORWARD_RIGHT.getButtonText());
		distanceRightShort = generateButton(Diagram.DISTANCE_RIGHT_SHORT.getButtonText());
		distanceRightLong = generateButton(Diagram.DISTANCE_RIGHT_LONG.getButtonText());
		tape = generateButton(Diagram.TAPE.getButtonText());
		controlError = generateButton(Diagram.CONTROL_ERROR.getButtonText());

		group = new ToggleGroup();
		distanceLeftShort.setToggleGroup(group);
		distanceLeftLong.setToggleGroup(group);
		distanceForwardLeft.setToggleGroup(group);
		distanceForwardCenter.setToggleGroup(group);
		distanceForwardRight.setToggleGroup(group);
		distanceRightShort.setToggleGroup(group);
		distanceRightLong.setToggleGroup(group);
		tape.setToggleGroup(group);
		controlError.setToggleGroup(group);
		
		r1.getChildren().addAll(distanceForwardLeft, distanceForwardCenter, distanceForwardRight);
		r2.getChildren().addAll(distanceLeftLong, tape, distanceRightLong);
		r3.getChildren().addAll(distanceLeftShort, controlError, distanceRightShort);

		this.getChildren().addAll(r1, r2, r3);
		
		distanceForwardCenter.setSelected(true);
		
		map = new HashMap<>();
		map.put(distanceLeftShort, Diagram.DISTANCE_LEFT_SHORT);
		map.put(distanceLeftLong, Diagram.DISTANCE_LEFT_LONG);
		map.put(distanceForwardLeft, Diagram.DISTANCE_FORWARD_LEFT);
		map.put(distanceForwardCenter, Diagram.DISTANCE_FORWARD_CENTER);
		map.put(distanceForwardRight, Diagram.DISTANCE_FORWARD_RIGHT);
		map.put(distanceRightLong, Diagram.DISTANCE_RIGHT_LONG);
		map.put(distanceRightShort, Diagram.DISTANCE_RIGHT_SHORT);
		map.put(tape, Diagram.TAPE);
		map.put(controlError, Diagram.CONTROL_ERROR);
		
		group.selectedToggleProperty().addListener(new ChangeListener<Toggle>() {

			@Override
			public void changed(ObservableValue<? extends Toggle> arg0,
					Toggle arg1, Toggle arg2) {
				if (arg2 == null) {
					arg1.setSelected(true);
				} else {
					callback.callback();
				}
			}
		});
	}

	private ToggleButton generateButton(String text) {
		ToggleButton b = new ToggleButton(text);
		b.setStyle("-fx-font: 20 arial;");
		
		setFocusTraversable(false);
		
		return b;
	}

	/**
	 * @return The type of LineChart that should be used.
	 */
	public Diagram getSelected() {
		return map.get(group.getSelectedToggle());
	}
}
