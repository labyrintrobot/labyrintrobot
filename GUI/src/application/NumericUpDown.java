package application;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

public class NumericUpDown extends HBox {
	
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
		this.leftBox = new VBox();
		this.rightBox = new VBox();
		
		this.up = new Button("↑");
		this.down = new Button("↓");
		
		this.label = new Label(title);
		this.label.setAlignment(Pos.CENTER);
		this.textField = new TextField(Integer.toString(value));
		this.textField.setEditable(false);
		this.textField.setAlignment(Pos.CENTER_LEFT);
		
		this.leftBox.getChildren().addAll(this.label, this.textField);
		this.rightBox.getChildren().addAll(up, down);
		this.textField.setAlignment(Pos.CENTER_RIGHT);
		
		this.getChildren().addAll(this.leftBox, this.rightBox);
		
		this.up.setOnAction(new EventHandler<ActionEvent>() {
			@Override public void handle(ActionEvent e) {
				int val = Integer.parseInt(NumericUpDown.this.textField.getText());
				if (val != max) {
					NumericUpDown.this.textField.setText(Integer.toString(val + 1));
					callback.valueChanged(Integer.parseInt(NumericUpDown.this.textField.getText()));
				}
			}
		});
		
		this.down.setOnAction(new EventHandler<ActionEvent>() {
			@Override public void handle(ActionEvent e) {
				int val = Integer.parseInt(NumericUpDown.this.textField.getText());
				if (val != min) {
					NumericUpDown.this.textField.setText(Integer.toString(val - 1));
					callback.valueChanged(Integer.parseInt(NumericUpDown.this.textField.getText()));
				}
			}
		});
	}
}
