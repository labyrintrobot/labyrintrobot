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

public class NumericUpDown extends HBox {
	
	private final int min;
	private final int max;
	private final ICallback callback;
	
	private final Label label;
	private final VBox rightBox;
	private final VBox leftBox;
	private final Button up;
	private final Button down;
	private final TextField textField;
	
	public interface ICallback {
		public void valueChanged(int newValue);
	}
	
	public NumericUpDown(String title, final ICallback callback, int value, final int min, final int max) {
		
		if (value < min || value > max) {
			throw new IllegalArgumentException();
		}
		
		this.callback = callback;
		this.min = min;
		this.max = max;
		
		this.leftBox = new VBox();
		this.rightBox = new VBox();
		
		this.up = new Button("↑");
		this.down = new Button("↓");
		
		this.label = new Label(title);
		//this.label.textAlignmentProperty().set(TextAlignment.CENTER);
		this.textField = new TextField(Integer.toString(value));
		//this.textField.setEditable(false);
		this.textField.setAlignment(Pos.CENTER_LEFT);
		
		this.leftBox.getChildren().addAll(this.label, this.textField);
		this.rightBox.getChildren().addAll(up, down);
		this.textField.setAlignment(Pos.CENTER_RIGHT);
		
		this.getChildren().addAll(this.leftBox, this.rightBox);
		
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
	}
	
	private void checkText(int oldVal, int newVal) {
		newVal = Math.max(min, newVal);
		newVal = Math.min(max, newVal);
		NumericUpDown.this.textField.setText("" + newVal);
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
