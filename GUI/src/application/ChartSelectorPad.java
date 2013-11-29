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

public class ChartSelectorPad extends VBox {
	
	public interface ToggleCallback {
		public void callback(SelectedToggleButton stb);
	}
	
	enum SelectedToggleButton {
		DISTANCE_LEFT_SHORT,
		DISTANCE_LEFT_LONG,
		DISTANCE_FORWARD_LEFT,
		DISTANCE_FORWARD_CENTER,
		DISTANCE_FORWARD_RIGHT,
		DISTANCE_RIGHT_LONG,
		DISTANCE_RIGHT_SHORT,
		TAPE
	}
	
	private final Map<Toggle, SelectedToggleButton> map;

	final ToggleGroup group;

	private final ToggleButton distanceLeftShort;
	private final ToggleButton distanceLeftLong;
	private final ToggleButton distanceForwardLeft;
	private final ToggleButton distanceForwardCenter;
	private final ToggleButton distanceForwardRight;
	private final ToggleButton distanceRightLong;
	private final ToggleButton distanceRightShort;
	private final ToggleButton tape;

	public ChartSelectorPad(final ToggleCallback callback) {

		this.setAlignment(Pos.CENTER);

		HBox r1 = new HBox();
		HBox r2 = new HBox();
		HBox r3 = new HBox();

		r1.setAlignment(Pos.TOP_CENTER);
		r2.setAlignment(Pos.CENTER);
		r3.setAlignment(Pos.BOTTOM_CENTER);

		distanceLeftShort = generateButton("1.2");
		distanceLeftLong = generateButton("2.1");
		distanceForwardLeft = generateButton("2.2");
		distanceForwardCenter = generateButton("1.1");
		distanceForwardRight = generateButton("2.3");
		distanceRightShort = generateButton("2.4");
		distanceRightLong = generateButton("1.3");
		tape = generateButton("Tape");

		group = new ToggleGroup();
		distanceLeftShort.setToggleGroup(group);
		distanceLeftLong.setToggleGroup(group);
		distanceForwardLeft.setToggleGroup(group);
		distanceForwardCenter.setToggleGroup(group);
		distanceForwardRight.setToggleGroup(group);
		distanceRightShort.setToggleGroup(group);
		distanceRightLong.setToggleGroup(group);
		tape.setToggleGroup(group);
		
		r1.getChildren().addAll(distanceForwardLeft, distanceForwardCenter, distanceForwardRight);
		r2.getChildren().addAll(distanceLeftLong, tape, distanceRightLong);
		r3.getChildren().addAll(distanceLeftShort, distanceRightShort);

		this.getChildren().addAll(r1, r2, r3);
		
		distanceForwardCenter.setSelected(true);
		
		map = new HashMap<>();
		map.put(distanceLeftShort, SelectedToggleButton.DISTANCE_LEFT_SHORT);
		map.put(distanceLeftLong, SelectedToggleButton.DISTANCE_LEFT_LONG);
		map.put(distanceForwardLeft, SelectedToggleButton.DISTANCE_FORWARD_LEFT);
		map.put(distanceForwardCenter, SelectedToggleButton.DISTANCE_FORWARD_CENTER);
		map.put(distanceForwardRight, SelectedToggleButton.DISTANCE_FORWARD_RIGHT);
		map.put(distanceRightLong, SelectedToggleButton.DISTANCE_RIGHT_LONG);
		map.put(distanceRightShort, SelectedToggleButton.DISTANCE_RIGHT_SHORT);
		map.put(tape, SelectedToggleButton.TAPE);
		
		group.selectedToggleProperty().addListener(new ChangeListener<Toggle>() {

			@Override
			public void changed(ObservableValue<? extends Toggle> arg0,
					Toggle arg1, Toggle arg2) {
				callback.callback(map.get(group.getSelectedToggle()));
			}
		});
	}

	private ToggleButton generateButton(String text) {
		ToggleButton b = new ToggleButton(text);
		b.setStyle("-fx-font: 26 arial;");
		
		setFocusTraversable(false);
		
		return b;
	}

	public SelectedToggleButton getSelected() {
		return map.get(group.getSelectedToggle());
	}
}
