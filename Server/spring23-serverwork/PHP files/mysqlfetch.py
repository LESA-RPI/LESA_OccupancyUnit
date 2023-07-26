#Code taken from https://pynative.com/python-mysql-select-query-to-fetch-data/ and modified for our purposes.

import mysql.connector


def GeneratePath(time1, time2):
    try:
        
        connection = mysql.connector.connect(host='localhost',
                                                database='occupancy',
                                                user='occupancy_user',
                                                password='#Occ_LESA_ToF#')
        sql_select_Query = "select * from sensor_data_pathgen WHERE Time_Captured BETWEEN " + "'" + time1 + "' AND '" + time2 + "' ORDER BY Time_Captured "
        cursor = connection.cursor()

            # set variable in query
        cursor.execute(sql_select_Query)
            # fetch result
        pathFound = cursor.fetchall()

        print("The person walked across the following building sensors: ")
        for row in pathFound:
            print(row[2])
        
        print("At the moment that the occupant left the sensor %s, they were moving at the %s angle, and at a speed of %s." % (pathFound[-1][2],pathFound[-1][6],pathFound[-1][5]))


    except mysql.connector.Error as error:
        print("Failed to get record from MySQL table: {}".format(error))

    finally:
        if connection.is_connected():
            cursor.close()
            connection.close()
            print("MySQL connection is closed")

#GeneratePath("2023-04-12 00:00:00", "2023-04-12 11:00:00")
print("Please input when to start searching under the format YYYY-MM-DD HH:MM:SS")
date1 = input()
print("Please input when to stop searching under the format YYYY-MM-DD HH:MM:SS")
date2 = input()
GeneratePath(date1, date2)

