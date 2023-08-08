#Code taken from https://pynative.com/python-mysql-select-query-to-fetch-data/ and modified for our purposes.

import mysql.connector

try:
    
    connection = mysql.connector.connect(host='localhost',
                                             database='occupancy',
                                             user='occupancy_user',
                                             password='#Occ_LESA_ToF#')
    sql_select_Query = "select * from Laptop"
    cursor = connection.cursor()

        # set variable in query
    cursor.execute(sql_select_Query)
        # fetch result
    record = cursor.fetchall()

    for row in record:
        for col in row:
            print(col + " ")
    print("\n")

except mysql.connector.Error as error:
    print("Failed to get record from MySQL table: {}".format(error))

finally:
    if connection.is_connected():
        cursor.close()
        connection.close()
        print("MySQL connection is closed")

get_laptop_detail(1)
get_laptop_detail(2)