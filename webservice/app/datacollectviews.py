import json
import logging
import os.path
from datetime import datetime

import win32api
import win32event
from django.http import JsonResponse

from .errorcode import ErrorCode
from .utils import *

# data路径
g_data_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'data')
os.makedirs(g_data_path, exist_ok=True)

logger = logging.getLogger('collectdata')


# 更新访问列表
def update_visit_list(client_ip):
    if len(client_ip) == 0:
        return False

    # 创建或获取命名互斥体
    mutex = None
    try:
        mutex = win32event.CreateMutex(None, False, 'Global\\VisitListJsonFileSyncMutex')
        # 等待获取互斥体
        wait_result = win32event.WaitForSingleObject(mutex, 5000)

        if wait_result != win32event.WAIT_OBJECT_0:
            logger.error('failed to wait the global mutex')
            return False

        # 加载文件
        visit_datas = []
        file_path = os.path.join(g_data_path, 'visit.txt')
        if os.path.exists(file_path):
            try:
                with open(file_path, 'r') as f:
                    visit_datas = json.load(f)
            except json.JSONDecodeError:
                logger.error('failed to load the visit data')
                return False

        # 更新或添加访问
        found = False
        for visit_data in visit_datas:
            if visit_data['ip'] == client_ip:
                found = True
                visit_data['time'] = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                break
        if not found:
            visit_data = {
                'ip': client_ip,
                'time': datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            }
            visit_datas.append(visit_data)

        # 写入更新后的数据
        with open(file_path, 'w') as f:
            json.dump(visit_datas, f, indent=4)

        return True
    except Exception as e:
        logger.error(f'failed to update visit list, error: {e}')
        return False
    finally:
        # 确保释放互斥体
        if mutex:
            try:
                win32event.ReleaseMutex(mutex)
                win32api.CloseHandle(mutex)
            except Exception as e:
                logger.error(f'failed to release the the global mutex, error: {e}')


def collect_data(request):
    try:
        request_body = json.loads(request.body)
        event_name = request_body['event_name']
        client_ip = request.META.get('REMOTE_ADDR')
        logger.info(f'the computer({client_ip}) send an event: {event_name}')

        if event_name == 'launch':
            update_visit_list(client_ip)

        response_body = make_response_body()
        return JsonResponse(response_body)
    except Exception as e:
        logger.error("collect data have an error: {}".format(e))
        body = make_response_body(code=ErrorCode.WRONG_PARAM, msg=ErrorCode.WRONG_PARAM_MSG)
        return JsonResponse(body)
