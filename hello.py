from flask import Flask, render_template, make_response, request, url_for, redirect
import random
import sys
import os
import glob

app = Flask(__name__)

@app.route('/')
def temp_page():
    response = make_response(render_template('main.html',
                camera = url_for('camera'), led = url_for('led'), water = url_for('water')))
    return response
    
@app.route('/camera')
def camera():
    response = make_response(render_template('camera.html',
                camera_1 = url_for('camera_1'), camera_2 = url_for('camera_2'),
                camera_3 = url_for('camera_3'), camera_4 = url_for('camera_4'),
                camera_5 = url_for('camera_5'), back = url_for('temp_page')))
    return response

@app.route('/led')
def led():
    response = make_response(render_template('led.html',
                led_1 = url_for('led_1'), led_2 = url_for('led_2'),
                led_3 = url_for('led_3'), back = url_for('temp_page')))
    return response

@app.route('/water')
def water():
    response = make_response(render_template('water.html',
                water_1 = url_for('water_1'), water_2 = url_for('water_2'),
                water_3 = url_for('water_3'), back = url_for('temp_page')))
    return response

@app.route('/camera_1')
def camera_1():

    pictures = glob.glob(os.path.join(os.getcwd(), "static/img/*.jpg"))
    i=0
    for picture in pictures:
        pictures[i] = os.path.basename(pictures[i])
        i+=1
    pictures.sort()

    response = make_response(render_template('picture_list.html', back = url_for('camera'),
                                             image = url_for('image'), pictures = pictures))
    return response

@app.route('/camera_2')
def camera_2():
    os.system(os.path.join(os.getcwd(), "static/img/camera"))

    pictures = glob.glob(os.path.join(os.getcwd(), "static/img/*.jpg"))
    i=0
    for picture in pictures:
        pictures[i] = os.path.basename(pictures[i])
        i+=1
    pictures.sort()

    response = make_response(render_template('catch.html', picture = url_for('temp_page')+"static/img/"+pictures[len(pictures)-1],
                                             back = url_for('camera')))
    return response

@app.route('/camera_3')
def camera_3():
    return redirect(url_for('minute'))

@app.route('/camera_4')
def camera_4():
    os.system("crontab -r")
    f = open("camera.txt", 'w')
    f.close()
    what = 'c'
    response = make_response(render_template('cancle.html', back = url_for('camera'), what = 'C'))
    return response

@app.route('/camera_5')
def camera_5():
    f = open("camera.txt", 'r')
    minute = ""
    hour = ""
    week = ""
    check = 0
    asc = f.read()
    if len(asc) == 0:
        response = make_response(render_template('time_of_camera_fail.html'), back=url_for('camera'))
        return response
    while asc[check] != ' ':
        check = check + 1
    minute = asc[0:check]
    asc = asc[check+1:]
    check = 0
    while asc[check] != ' ':
        check = check + 1
    hour = asc[0:check]
    asc = asc[check+5:]
    check = 0
    while asc[check] != ' ':
        check = check + 1
    week = asc[0:check]

    response = make_response(render_template('time_of_camera.html'), week=week, hour=hour, minute=minute,
                             back = url_for('camera'))
    return response

@app.route('/image')
@app.route('/image/<name>')
def image(name):
    response = make_response(render_template('image.html', name=name, delete=url_for('delete_page'),
                                             list=url_for('camera_1'), back=url_for('temp_page')))
    return response

@app.route('/delete_page')
@app.route('/delete_page/<name>')
def delete_page(name):
    response = make_response(render_template('delete_page.html', delete=url_for('delete'),
                                             name=name, list=url_for('camera_1')))
    return response

@app.route('/delete')
@app.route('/delete/<name>')
def delete(name):
    os.system("rm -r ./static/img/"+name)
    return redirect(url_for('camera_1'))

@app.route('/minute')
def minute():
    response = make_response(render_template('minute.html', cal = url_for('minute_cal')))
    return response

@app.route('/minute_cal', methods=['POST'])
def minute_cal():
    f = open("camera.txt", 'w')

    list = request.form.getlist('o[]')
    str = ""

    if list:
        start = 1
        str = str + list[0]
        while start < len(list):
            str = str + "," + list[start]
            start = start + 1
    else:
        str = "*"

    f.write(str)

    f.close()
    return redirect(url_for('hour'))

@app.route('/hour')
def hour():
    response = make_response(render_template('hour.html', cal = url_for('hour_cal')))
    return response

@app.route('/hour_cal', methods=['POST'])
def hour_cal():
    f = open("camera.txt", 'a')
    f.write(" ")

    list = request.form.getlist('o[]')
    str = ""

    if list:
        start = 1
        str = str + list[0]
        while start < len(list):
            str = str + "," + list[start]
            start = start + 1
    else:
        str = "*"

    f.write(str)

    f.close()
    return redirect(url_for('week'))

@app.route('/week')
def week():
    response = make_response(render_template('week.html', cal = url_for('week_cal')))
    return response

@app.route('/week_cal', methods=['POST'])
def week_cal():
    f = open("camera.txt", 'a')
    f.write(" * * ")

    list = request.form.getlist('o[]')
    str = ""

    if list:
        start = 1
        str = str + list[0]
        while start < len(list):
            str = str + "," + list[start]
            start = start + 1
    else:
        str = "*"

    f.write(str+ " " +os.path.join(os.getcwd(), "static/img/camera\n"))

    f.close()
    os.system("crontab -r")
    os.system("crontab camera.txt")
    return redirect(url_for('camera'))

@app.route('/led_1')
def led_1():
    response = make_response(render_template('led_1.html', cal = url_for('led_1_cal')))
    return response

@app.route('/led_2')
def led_2():
    os.system("/home/pi/embeded/dht -200 100")
    response = make_response(render_template('cancle.html', back = url_for('led'), what = 'L'))
    return response

@app.route('/led_3')
def led_3():
    return redirect(url_for('led'))

@app.route('/led_1_cal', methods=['POST'])
def led_2_cal():
    os.system("/home/pi/embeded/dht "+request.form['degree']+" 100")
    return redirect('led')


@app.route('/water_1')
def water_1():
    return redirect(url_for('water'))

@app.route('/water_2')
def water_2():
    return redirect(url_for('water'))

@app.route('/water_3')
def water_3():
    response = make_response(render_template('cancle.html', back = url_for('water'), what = 'W'))
    return response

@app.route('/water_4')
def water_4():
    return redirect(url_for('water'))

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000)
