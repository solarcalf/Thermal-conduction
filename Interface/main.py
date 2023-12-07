from kivy.app import App
from kivy.uix.label import Label
from kivy.uix.button import Button
from kivy.graphics import Color, Rectangle
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.textinput import TextInput
from kivy.uix.gridlayout import GridLayout
from kivy.uix.scrollview import ScrollView
from kivy.uix.screenmanager import ScreenManager, Screen
from kivy.garden.matplotlib.backend_kivyagg import FigureCanvasKivyAgg
import matplotlib.pyplot as plt
import csv
import subprocess


class SecondScreen(Screen):
    param1: int
    x_vals: list
    xh_vals: list
    y_vals: list
    yh_vals: list
    e_vals: list
    plot_type: str

    def __init__(self, **kwargs):
        super(SecondScreen, self).__init__(**kwargs)

        super().__init__()
        self.param1 = 0
        self.x_vals = []
        self.xh_vals = []
        self.y_vals = []
        self.yh_vals = []
        self.e_vals = []
        self.plot_type = "plus"

        with self.canvas:
            Color(0, 0, 0, 1)
            self.rect = Rectangle(size=self.size, pos=self.pos)

        self.bind(size=self._update_rect, pos=self._update_rect)

        # The main layout of the application
        layout = BoxLayout(orientation='horizontal', padding=10, spacing=5)
        vertical_box = BoxLayout(orientation='vertical', spacing=5)

        # Add interaction logic between windows
        label = Label(text="Основная задача", size_hint=(None, None), size=(200, 50), color=[1, 1, 1, 1])
        label_input = Label(text='Число разбиений: ', size_hint=(None, None), size=(200, 50), color=[1, 1, 1, 1])
        button = Button(text="К тестовой задаче", on_press=self.switch_screen,
                        size_hint=(None, None), size=(200, 50), color=[0, 0, 0, 1])

        # Create a widget for help output
        text = ("\n"
                f"Для решения задачи использована равномерная сетка с числом разбиений n = {self.param1};\n"
                "\n"
                "Задача должна быть решена с погрешностью не более ε = 0.5e-6;\n"
                "\n"
                "Задача решена с погрешностью ε1 = __;\n"
                "\n"
                "Максимальное отклонение аналитического и численного решений наблюдается в точке x=__")

        # Create a Label widget with text and set the size
        self.label_text = Label(text=text, size_hint_y=None, valign='top', markup=True)
        self.label_text.bind(texture_size=self.label_text.setter('size'))

        # Create a ScrollView and add a Label to it
        self.scroll_view_text = ScrollView(size_hint=(1, None), size=(300, 200), do_scroll_x=False)
        self.scroll_view_text.add_widget(self.label_text)

        # Fields for entering parameters
        parameter1 = TextInput(multiline=False, size_hint=(None, None), size=(150, 30), hint_text='n')

        # Table for displaying data
        self.table_layout = GridLayout(cols=5, spacing=5, size_hint_y=None, row_default_height=40)
        self.table_layout.bind(minimum_height=self.table_layout.setter('height'))
        self.scroll_view = ScrollView(size_hint=(0.4, 1), do_scroll_y=True, do_scroll_x=False,
                                      scroll_type=['bars'], bar_width='15dp')
        self.scroll_view.add_widget(self.table_layout)

        # Adding a field for a graph
        self.fir, self.ax = plt.subplots()

        # Handler for the parameter input event
        def on_text(instance):
            try:
                self.param1 = int(parameter1.text)

                with open("Source.txt", "w") as file:
                    file.write(str(self.param1))

                cpp_executable = "Release/ThermalConduction.exe"
                args = [cpp_executable]
                try:
                    subprocess.run(args, check=True)
                except subprocess.CalledProcessError:
                    print("Error when starting a C++ project")
                except FileNotFoundError:
                    print("ThermalConduction.exe not found")

                with open('x_grid.txt', 'r') as file:
                    self.x_vals = [float(line.strip()) for line in file]
                with open('x_half_grid.txt', 'r') as file:
                    self.xh_vals = [float(line.strip()) for line in file]
                with open('v_values.txt', 'r') as file:
                    self.y_vals = [float(line.strip()) for line in file]
                with open('v_half_values.txt', 'r') as file:
                    self.yh_vals = [float(line.strip()) for line in file]
                with open('e_vals_main.txt', 'r') as file:
                    self.e_vals = [float(line.strip()) for line in file]

                text = ("\n"
                        f"Для решения задачи использована равномерная сетка с числом разбиений n = {self.param1};\n"
                        "\n"
                        "Задача должна быть решена с погрешностью не более ε = 0.5e-6;\n"
                        "\n"
                        f"Задача решена с погрешностью ε1 = {max(self.e_vals)};\n"
                        "\n"
                        "Максимальное отклонение аналитического и численного решений наблюдается в точке x="
                        f"{self.x_vals[self.e_vals.index(max(self.e_vals))]}")
                self.label_text.text = text

                self.ax.clear()
                # Build a graph
                self.ax.plot(self.x_vals, self.y_vals, label="v(xi)")
                self.ax.plot(self.xh_vals, self.yh_vals, label="v2(xi)")  # second plot
                self.ax.set_title("График численного решения (синяя линия) и численного решения с половинным шагом (рыжая линия)")
                self.ax.set_xlabel('xi')
                self.ax.grid(True)
                self.ax.figure.canvas.draw()

                u_vals = []
                nh = 0
                for n in range(0, self.param1 + 1):
                    u_vals.append(self.yh_vals[nh])
                    nh += 2

                # Build a table
                self.table_layout.clear_widgets()
                self.add_table_data(self.x_vals, self.y_vals, u_vals, self.e_vals)
            except ValueError:
                pass

        # Process the press of the Enter key
        parameter1.bind(on_text_validate=on_text)

        # Create a button to switch the chart
        switch_button = Button(text='Переключить график', size_hint=(None, None), size=(200, 50))
        switch_button.bind(on_press=self.switch_graph)

        # Add widgets to the layout
        grid = GridLayout(cols=5)
        vertical_box.add_widget(label)
        vertical_box.add_widget(button)
        vertical_box.add_widget(label_input)
        vertical_box.add_widget(parameter1)
        vertical_box.add_widget(switch_button)
        vertical_box.add_widget(self.scroll_view_text)
        vertical_box.add_widget(FigureCanvasKivyAgg(plt.gcf()))
        layout.add_widget(vertical_box)
        grid.add_widget(self.scroll_view)
        layout.add_widget(grid)
        self.add_widget(layout)


    def switch_graph(self, instance):
        title = ""
        if self.plot_type == "minus":
            self.plot_type = "plus"
            title = "График численного решения (синяя линия) и численного решения с половинным шагом (рыжая линия)"
            self.ax.clear()
            # Build a graph
            self.ax.plot(self.x_vals, self.y_vals)
            self.ax.plot(self.xh_vals, self.yh_vals)  # second plot
            self.ax.set_title(title)
            self.ax.set_xlabel('x')
            self.ax.grid(True)
            self.ax.figure.canvas.draw()
        else:
            self.plot_type = "minus"
            title = "График разности численных решений в общих узлах"

            self.ax.clear()
            # Build a graph
            self.ax.plot(self.x_vals, self.e_vals)
            self.ax.set_title(title)
            self.ax.set_xlabel('x')
            self.ax.grid(True)
            self.ax.figure.canvas.draw()

    def add_table_data(self, x_values, u_values, v_values, e_values):
        # Add data to the table
        for value in ['№ узла', 'xi', 'v(xi)', 'v2(xi)', 'v(xi) - v2(xi)']:
            cell = Label(text=str(value), size_hint_x=None, width=150, color=[0, 0, 0, 1])
            cell.bind(size=self.draw_border)
            self.table_layout.add_widget(cell)
        num = 0
        for x, u, v, e in zip(x_values, u_values, v_values, e_values):
            num += 1
            for value in [num, x, u, v, e]:
                cell = Label(text=str(value), size_hint_x=None, width=150, color=[0, 0, 0, 1])
                cell.bind(size=self.draw_border)
                self.table_layout.add_widget(cell)

    def draw_border(self, instance, size):
        instance.canvas.before.clear()
        with instance.canvas.before:
            Color(1, 1, 1, 1)
            instance.rect = Rectangle(size=instance.size, pos=instance.pos)
        instance.bind(size=self._update_rect, pos=self._update_rect)

    def _update_rect(self, instance, value):
        self.rect.size = instance.size
        self.rect.pos = instance.pos

    def switch_screen(self, *args):
        self.manager.current = 'first'


class FirstScreen(Screen):
    param1: int
    x_vals: list
    v_vals: list
    u_vals: list
    e_vals: list
    plot_type: str

    def __init__(self, **kwargs):
        super(FirstScreen, self).__init__(**kwargs)

        super().__init__()
        self.param1 = 0
        self.x_vals = []
        self.v_vals = []
        self.u_vals = []
        self.e_vals = []
        self.plot_type = "plus"

        with self.canvas:
            Color(0, 0, 0, 1)
            self.rect = Rectangle(size=self.size, pos=self.pos)

        self.bind(size=self._update_rect, pos=self._update_rect)

        # The main layout of the application
        layout = BoxLayout(orientation='horizontal', padding=10, spacing=5)
        vertical_box = BoxLayout(orientation='vertical', spacing=5)

        # Add interaction logic between windows
        label = Label(text="Тестовая задача", size_hint=(None, None), size=(200, 50), color=[1, 1, 1, 1])
        label_input = Label(text='Число разбиений: ', size_hint=(None, None), size=(200, 50), color=[1, 1, 1, 1])
        button = Button(text="К основной задаче", on_press=self.switch_screen,
                        size_hint=(None, None), size=(200, 50), color=[0, 0, 0, 1])

        # Create a widget for help output
        text = ("\n"
                f"Для решения задачи использована равномерная сетка с числом разбиений n = {self.param1};\n"
                "\n"
                "Задача должна быть решена с погрешностью не более ε = 0.5e-6;\n"
                "\n"
                f"Задача решена с погрешностью ε1 = __;\n"
                "\n"
                f"Максимальное отклонение аналитического и численного решений наблюдается в точке x=__")

        # Create a Label widget with text and set the size
        self.label_text = Label(text=text, size_hint_y=None, valign='top', markup=True)
        self.label_text.bind(texture_size=self.label_text.setter('size'))

        # Create a ScrollView and add a Label to it
        self.scroll_view_text = ScrollView(size_hint=(1, None), size=(300, 200), do_scroll_x=False)
        self.scroll_view_text.add_widget(self.label_text)

        # Fields for entering parameters
        parameter1 = TextInput(multiline=False, size_hint=(None, None), size=(150, 30), hint_text='n')

        # Table for displaying data
        self.table_layout = GridLayout(cols=5, spacing=5, size_hint_y=None, row_default_height=40)
        self.table_layout.bind(minimum_height=self.table_layout.setter('height'))
        self.scroll_view = ScrollView(size_hint=(0.4, 1), do_scroll_y=True, do_scroll_x=False,
                                      scroll_type=['bars'], bar_width='15dp')
        self.scroll_view.add_widget(self.table_layout)

        # Adding a field for a graph
        self.fir, self.ax = plt.subplots()

        # Handler for the parameter input event
        def on_text(instance):
            try:
                self.param1 = int(parameter1.text)

                with open("Source.txt", "w") as file:
                    file.write(str(self.param1))

                cpp_executable = "Release/ThermalConduction.exe"
                args = [cpp_executable]
                try:
                    subprocess.run(args, check=True)
                except subprocess.CalledProcessError:
                    print("Error when starting a C++ project")
                except FileNotFoundError:
                    print("ThermalConduction.exe not found")

                with open('x_grid.txt', 'r') as file:
                    self.x_vals = [float(line.strip()) for line in file]
                with open('v_values.txt', 'r') as file:
                    self.v_vals = [float(line.strip()) for line in file]
                with open('u_values.txt', 'r') as file:
                    self.u_vals = [float(line.strip()) for line in file]
                with open('e_vals_test.txt', 'r') as file:
                    self.e_vals = [float(line.strip()) for line in file]

                text = ("\n"
                        f"Для решения задачи использована равномерная сетка с числом разбиений n = {self.param1};\n"
                        "\n"
                        "Задача должна быть решена с погрешностью не более ε = 0.5e-6;\n"
                        "\n"
                        f"Задача решена с погрешностью ε1 = {max(self.e_vals)};\n"
                        "\n"
                        f"Максимальное отклонение аналитического и численного решений наблюдается в точке x= "
                        f"{self.x_vals[self.e_vals.index(max(self.e_vals))]}")
                self.label_text.text = text

                self.ax.clear()
                # Build a graph
                self.ax.plot(self.x_vals, self.v_vals)
                self.ax.plot(self.x_vals, self.u_vals)
                self.ax.set_title("График аналитического (рыжая линия) и численного решения (синяя линия)")
                self.ax.set_xlabel('xi')
                self.ax.grid(True)
                self.ax.figure.canvas.draw()

                # Build a table
                self.table_layout.clear_widgets()
                self.add_table_data(self.x_vals, self.u_vals, self.v_vals, self.e_vals)
            except ValueError:
                pass

        # Process the press of the Enter key
        parameter1.bind(on_text_validate=on_text)

        # Create a button to switch the chart
        switch_button = Button(text='Переключить график', size_hint=(None, None), size=(200, 50))
        switch_button.bind(on_press=self.switch_graph)

        # Add widgets to the layout
        grid = GridLayout(cols=5)
        vertical_box.add_widget(label)
        vertical_box.add_widget(button)
        vertical_box.add_widget(label_input)
        vertical_box.add_widget(parameter1)
        vertical_box.add_widget(switch_button)
        vertical_box.add_widget(self.scroll_view_text)
        vertical_box.add_widget(FigureCanvasKivyAgg(plt.gcf()))
        layout.add_widget(vertical_box)
        grid.add_widget(self.scroll_view)
        layout.add_widget(grid)
        self.add_widget(layout)

    def switch_graph(self, instance):
        title = ""
        if self.plot_type == "minus":
            self.plot_type = "plus"
            title = "График аналитического (рыжая линия) и численного решения (синяя линия)"
            self.ax.clear()
            # Build a graph
            self.ax.plot(self.x_vals, self.v_vals)
            self.ax.plot(self.x_vals, self.u_vals)  # second plot
            self.ax.set_title(title)
            self.ax.set_xlabel('x')
            self.ax.grid(True)
            self.ax.figure.canvas.draw()
        else:
            self.plot_type = "minus"
            title = "График разности аналитического и численного решения"
            e_values = []
            self.ax.clear()
            # Build a graph
            self.ax.plot(self.x_vals, self.e_vals)
            self.ax.set_title(title)
            self.ax.set_xlabel('x')
            self.ax.grid(True)
            self.ax.figure.canvas.draw()

    def add_table_data(self, x_values, u_values, v_values, e_values):
        # Add data to the table
        for value in ['№ узла', 'xi', 'u(xi)', 'v(xi)', 'u(xi) - v(xi)']:
            cell = Label(text=str(value), size_hint_x=None, width=150, color=[0, 0, 0, 1])
            cell.bind(size=self.draw_border)
            self.table_layout.add_widget(cell)
        num = 0
        for x, u, v, e in zip(x_values, u_values, v_values, e_values):
            num += 1
            for value in [num, x, u, v, e]:
                cell = Label(text=str(value), size_hint_x=None, width=150, color=[0, 0, 0, 1])
                cell.bind(size=self.draw_border)
                self.table_layout.add_widget(cell)

    def draw_border(self, instance, size):
        instance.canvas.before.clear()
        with instance.canvas.before:
            Color(1, 1, 1, 1)
            instance.rect = Rectangle(size=instance.size, pos=instance.pos)
        instance.bind(size=self._update_rect, pos=self._update_rect)

    def _update_rect(self, instance, value):
        self.rect.size = instance.size
        self.rect.pos = instance.pos

    def switch_screen(self, *args):
        self.manager.current = 'second'


class ParameterInputApp(App):
    def _update_rect(self, instance, value):
        self.rect.size = instance.size
        self.rect.pos = instance.pos

    def switch_screen(self, *args):
        self.manager.current = 'first'

    def build(self):
        screen_manager = ScreenManager()

        first_screen = FirstScreen(name='first')
        second_screen = SecondScreen(name='second')

        screen_manager.add_widget(first_screen)
        screen_manager.add_widget(second_screen)

        return screen_manager


if __name__ == '__main__':
    ParameterInputApp().run()
