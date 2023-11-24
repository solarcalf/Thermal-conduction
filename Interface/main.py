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


class SecondScreen(Screen):
    def __init__(self, **kwargs):
        super(SecondScreen, self).__init__(**kwargs)

        with self.canvas:
            Color(1, 1, 1, 1)
            self.rect = Rectangle(size=self.size, pos=self.pos)

        self.bind(size=self._update_rect, pos=self._update_rect)

        layout = BoxLayout(orientation='vertical')
        label = Label(text="This is the first screen")
        button = Button(text="Go to Second Screen", on_press=self.switch_screen)

        layout.add_widget(label)
        layout.add_widget(button)

        self.add_widget(layout)

    def _update_rect(self, instance, value):
        self.rect.size = instance.size
        self.rect.pos = instance.pos

    def switch_screen(self, *args):
        self.manager.current = 'first'


class FirstScreen(Screen):
    param1: float
    param2: float
    param3: float
    plot_type: str

    def __init__(self, **kwargs):
        super(FirstScreen, self).__init__(**kwargs)

        super().__init__()
        self.param1 = 0
        self.param2 = 0
        self.param3 = 0
        self.plot_type = "plus"

        with self.canvas:
            Color(0, 0, 0, 1)
            self.rect = Rectangle(size=self.size, pos=self.pos)

        self.bind(size=self._update_rect, pos=self._update_rect)

        # The main layout of the application
        layout = BoxLayout(orientation='horizontal', padding=10, spacing=5)
        vertical_box = BoxLayout(orientation='vertical', spacing=5)

        # Add interaction logic between windows
        label = Label(text="This is the first screen", size_hint=(None, None), size=(200, 50), color=[1, 1, 1, 1])
        button = Button(text="Go to Second Screen", on_press=self.switch_screen,
                        size_hint=(None, None), size=(200, 50), color=[0, 0, 0, 1])

        # Create a widget for help output
        text = ("\n"
                "Для решения задачи использована равномерная сетка с числом разбиений n = __;\n"
                "\n"
                "Задача должна быть решена с погрешностью не более ε = 0.5e-6;\n"
                "\n"
                "Задача решена с погрешностью ε1 = __;\n"
                "\n"
                "Максимальное отклонение аналитического и численного решений наблюдается в точке x=__")

        # Create a Label widget with text and set the size
        label_text = Label(text=text, size_hint_y=None, valign='top', markup=True)
        label_text.bind(texture_size=label_text.setter('size'))

        # Create a ScrollView and add a Label to it
        self.scroll_view_text = ScrollView(size_hint=(1, None), size=(300, 200), do_scroll_x=False)
        self.scroll_view_text.add_widget(label_text)

        # Fields for entering parameters
        parameter1 = TextInput(multiline=False, size_hint=(None, None), size=(150, 30), hint_text='Parameter 1')

        # Table for displaying data
        self.table_layout = GridLayout(cols=3, spacing=5, size_hint_y=None, row_default_height=40)
        self.table_layout.bind(minimum_height=self.table_layout.setter('height'))
        self.scroll_view = ScrollView(size_hint=(0.4, 1), do_scroll_y=True, do_scroll_x=False,
                                      scroll_type=['bars'], bar_width='15dp')
        self.scroll_view.add_widget(self.table_layout)

        # Adding a field for a graph
        self.fir, self.ax = plt.subplots()

        # Handler for the parameter input event
        def on_text(instance):
            try:
                self.param1 = float(parameter1.text)
                x_values = [x for x in range(-100, 101)]
                y_values = [self.param1 * x ** 2 for x in x_values]

                self.ax.clear()

                # Build a graph
                self.ax.plot(x_values, y_values)
                self.ax.grid(True)
                self.ax.figure.canvas.draw()

                # Build a table
                self.table_layout.clear_widgets()
                self.add_table_data(x_values, y_values)
            except ValueError:
                pass

        # Process the press of the Enter key
        parameter1.bind(on_text_validate=on_text)

        # Create a button to switch the chart
        switch_button = Button(text='Переключить график', size_hint=(None, None), size=(200, 50))
        switch_button.bind(on_press=self.switch_graph)

        # Add widgets to the layout
        grid = GridLayout(cols=3)
        vertical_box.add_widget(label)
        vertical_box.add_widget(button)
        vertical_box.add_widget(parameter1)
        vertical_box.add_widget(switch_button)
        vertical_box.add_widget(self.scroll_view_text)
        vertical_box.add_widget(FigureCanvasKivyAgg(plt.gcf()))
        layout.add_widget(vertical_box)
        grid.add_widget(self.scroll_view)
        layout.add_widget(grid)
        self.add_widget(layout)

    def switch_graph(self, instance):
        x_values = [x for x in range(-100, 101)]
        if self.plot_type == "minus":
            y_values = [self.param1 * x ** 2 for x in x_values]
            self.plot_type = "plus"
        else:
            y_values = [-self.param1 * x ** 2 for x in x_values]
            self.plot_type = "minus"

        # Build a new graph
        self.ax.clear()
        self.ax.plot(x_values, y_values)
        self.ax.grid(True)
        self.ax.figure.canvas.draw()

    def add_table_data(self, x_values, y_values):
        # Add data to the table
        for value in ['№ узла', 'X', 'Y']:
            cell = Label(text=str(value), size_hint_x=None, width=150, color=[0, 0, 0, 1])
            cell.bind(size=self.draw_border)
            self.table_layout.add_widget(cell)
        num = 0
        for x, y in zip(x_values, y_values):
            num += 1
            for value in [num, x, y]:
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
