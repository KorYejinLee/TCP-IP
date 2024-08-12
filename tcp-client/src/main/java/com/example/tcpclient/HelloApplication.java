package com.example.tcpclient;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.control.Slider;
import javafx.stage.Stage;

import java.io.IOException;

public class HelloApplication extends Application {
    @Override
    public void start(Stage stage) throws IOException {
        FXMLLoader fxmlLoader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
        Scene scene = new Scene(fxmlLoader.load(), 600, 400);
        stage.setTitle("Hello!");
        stage.setScene(scene);
        stage.show();
        HelloController helloController = fxmlLoader.getController();
        Slider redSlider = helloController.red_slider;
        Slider blueSlider = helloController.blue_slider;
        redSlider.setOnMouseClicked(e -> {
            helloController.slider_red_LED(redSlider);
        });
        blueSlider.setOnMouseClicked(e -> {
            helloController.slider_blue_LED(blueSlider);
        });
    }

    public static void main(String[] args) {
        launch();
    }
}