#!/usr/bin/python


import os
import MySQLdb
import sys
import datetime

db_host="10.173.13.40"
db_port=3306
db_name="spider_db"
db_user="root"
db_pwd="root40"

reload(sys)
sys.setdefaultencoding('utf-8')

yestoday = datetime.date.today() - datetime.timedelta(days=1)
#yestoday = datetime.date(2015,04,29)

def db_update(sql):
    try:
        conn = MySQLdb.connect(db_host, db_user, db_pwd, db_name, db_port, local_infile=1)
        c = conn.cursor()
        c.execute(sql)
        conn.commit()
        ret = c.rowcount
        c.close()
        return ret
    except Exception, e:
        print str(e)
        return 0


def create_domain_table(table_name):
    global yestoday
    #yestoday = datetime.date.today() - datetime.timedelta(days=1)
    #table_name = "domain_statistics_" + yestoday.strftime('%Y%m%d');
    print table_name
    sql = '''CREATE TABLE IF NOT EXISTS %s(
             id INTEGER PRIMARY KEY AUTO_INCREMENT, 
             domain VARCHAR(160), 
             start_time TIMESTAMP,
             req_sum INTEGER, 
             success_sum INTEGER, 
             deny_sum INTEGER, 
             expire_sum INTEGER, 
             cs_tm_sum INTEGER, 
             fail_sum INTEGER); ''' % table_name
    db_update(sql)

def create_downstream_table(table_name):
    global yestoday
    #table_name = "downstream_statistics_" + yestoday.strftime('%Y%m%d');
    print table_name
    sql = '''CREATE TABLE IF NOT EXISTS %s(
             id INTEGER PRIMARY KEY AUTO_INCREMENT, 
             ip_addr VARCHAR(160), 
             is_proxy INTEGER,
             start_time TIMESTAMP,
             req_sum INTEGER, 
             success_sum INTEGER, 
             expire_sum INTEGER, 
             fail_sum INTEGER,
             cs_tm_sum INTEGER); ''' % table_name
    db_update(sql)

def load_domain_data_into_db(local_file, table_name):
    print local_file
    sql = '''LOAD DATA LOCAL INFILE \"%s\" INTO 
             TABLE %s(domain, start_time, 
             req_sum, success_sum, deny_sum, expire_sum, 
             cs_tm_sum, fail_sum);''' % (local_file, table_name)
    ret = db_update(sql)
    print "Success Load %d Records From File %s Into %s" % (ret, local_file, table_name)

def load_downstream_data_into_db(local_file, table_name):
    print local_file
    sql = '''LOAD DATA LOCAL INFILE \"%s\" INTO 
             TABLE %s(ip_addr, is_proxy, start_time, req_sum, 
             success_sum, expire_sum, fail_sum, cs_tm_sum);''' % (local_file, table_name)
    ret = db_update(sql)
    print "Success Load %d Records From File %s Into %s" % (ret, local_file, table_name)

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print "Usage: %s type local_file cluster" % sys.argv[0]
        print "      type: domain or downstream"
        sys.exit(0)
    
    if os.path.exists(sys.argv[2]) == False:
        print "File %s Not Exists" % sys.argv[2]
        sys.exit(0)

    cluster = sys.argv[3];

    if sys.argv[1] == "domain":
        table_name = cluster + "_domain_statistics_" + yestoday.strftime('%Y%m%d')
        create_domain_table(table_name)
        load_domain_data_into_db(sys.argv[2], table_name)
    elif sys.argv[1] == "downstream":
        table_name = cluster + "_downstream_statistics_" + yestoday.strftime('%Y%m%d')
        create_downstream_table(table_name)
        load_downstream_data_into_db(sys.argv[2], table_name)
    else:
        print "Wrong Type, Type only has two choices(domain, downstream)"
        sys.exit(0)

    print "Done."
