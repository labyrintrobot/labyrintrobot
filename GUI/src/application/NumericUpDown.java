package application;

import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

/**
 * A simple numeric up down control (spinner). Since this control doesn't exist yet in JavaFX.
 * @author Emil Berg
 *
 */
public class NumericUpDown extends HBox {
	
	private final int min;
	private final int max;
	private final ICallback callback;
	
	private final Label label;
	private final VBox rightBox;
	private final Button up;
	private final Button down;
	private final TextField textField;
	
	public interface ICallback {
		public void valueChanged(int newValue);
	}
	
	public NumericUpDown(String text, final ICallback callback, int value, final int min, final int max) {
		
		if (value < min || value > max) {
			throw new IllegalArgumentException();
		}
		
		this.callback = callback;
		this.min = min;
		this.max = max;
		
		this.rightBox = new VBox();
		
		this.textField = new TextField(Integer.toString(value));
		//this.up = new Button("↑");
		//this.down = new Button("↓");
		this.up = new Button("+");
		this.down = new Button("-");
		
		this.label = new Label(text);
		this.label.setMinWidth(42);
		this.textField.setAlignment(Pos.CENTER_LEFT);
		
		this.rightBox.getChildren().addAll(up, down);
		this.textField.setAlignment(Pos.CENTER_RIGHT);
		
		this.getChildren().addAll(this.label, this.textField, this.rightBox);
		
		this.textField.textProperty().addListener(new ChangeListener<String>() {

			@Override
			public void changed(ObservableValue<? extends String> observable,
					String oldValue, String newValue) {
				int oldI;
				try {
					oldI = Integer.parseInt(oldValue);
				} catch (NumberFormatException e) {
					oldI = min;
				}
				int newI;
				try {
					newI = Integer.parseInt(newValue);
				} catch (NumberFormatException e) {
					newI = min;
				}
				checkText(oldI, newI);
			}
		});
		
		this.up.setOnAction(new EventHandler<ActionEvent>() {
			@Override public void handle(ActionEvent e) {
				int val = Integer.parseInt(NumericUpDown.this.textField.getText());
				checkText(val, val + 1);
			}
		});
		
		this.down.setOnAction(new EventHandler<ActionEvent>() {
			@Override public void handle(ActionEvent e) {
				int val = Integer.parseInt(NumericUpDown.this.textField.getText());
				checkText(val, val - 1);
			}
		});
		
		this.up.setStyle("-fx-font: 8 arial;");
		this.down.setStyle("-fx-font: 8 arial;");
		this.up.setMaxHeight(this.textField.getHeight() / 2);
		this.down.setMaxHeight(this.textField.getHeight() / 2);
		this.up.setMinWidth(24);
		this.down.setMinWidth(24);
	}
	
	private void checkText(int oldVal, int newVal) {
		newVal = Math.max(min, newVal);
		newVal = Math.min(max, newVal);
		NumericUpDown.this.textField.setText(Integer.toString(newVal));
		if (oldVal != newVal) {
			callback.valueChanged(newVal);
		}
	}
	
	public int getVal() {
		int i;
		try {
			i = Integer.parseInt(NumericUpDown.this.textField.getText());
			i = Math.max(min, i);
			i = Math.min(max, i);
		} catch (NumberFormatException e) {
			i = min;
		}
		return i;
	}
}
