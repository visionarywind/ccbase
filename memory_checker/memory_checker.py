import sys


def FindAndSlice(lst, target):
    try:
        index = lst.index(target)
        # print("remove", lst[:index + 1])
        return lst[index + 1 :]
    except ValueError:
        print(f"WARNING: event {target} not found!", flush=True)
        return lst


def _CheckAddrValidInMemory(addr, size, memory_info):
    if addr in memory_info:
        if size <= memory_info[addr]:
            return True
        return False
  
    addr_start = int(addr, 16)
    addr_end = addr_start + size
    for memory_addr, memory_size in memory_info.items():
        memory_start = int(memory_addr, 16)
        memory_end = memory_start + memory_size
        if addr_start >= memory_start and addr_end <= memory_end:
            return True
    return False

# 检查的思路
# 1 检查每对event的下发顺序，确保一定是先record再wait
# 2 对于非默认流的算子，检查执行前必然有record 0和wait 当前stream
# 3 对于默认流的算子，检查其和所有非默认流的非安全地址进行检测，判断是否存在重叠的区域
def LoadFileAndCheck(file_path):
    memory_info = dict()  # addr -> size
    event_info = dict()
    stream_launch_info = dict()
    stream_task_list = dict() # stream_id -> task list
    wait_info = [
      [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
      [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
      [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
      [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
      [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
      [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
      [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
      [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
      [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
      [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    ]
    with open(file_path, "r", encoding="utf-8") as file:
        process_count = 0
        for line in file:
            process_count = process_count + 1
            print("process ", process_count, line, flush=True)
            # load info and update memory info
            arr = line.split(",")
            if "memory_info" in line:
                # alloc or free, alloc - 0, free - 1
                is_alloc = arr[1] == "0"
                addr_size_str = arr[2].split("#")
                addr = addr_size_str[0]
                size = int(addr_size_str[1])
                if is_alloc:
                    # may be need check range ?
                    if addr in memory_info:
                        print(f"ERROR: {process_count} addr {addr} duplicated!", flush=True)
                        return
                    memory_info[addr] = size
                else:
                    if addr not in memory_info:
                        print(f"ERROR: {process_count} addr {addr} not found!", flush=True)
                    else:
                        del memory_info[addr]
            elif "event_info" in line:
                stream_id = arr[1]
                is_record = arr[2] == "record"
                event = arr[3]
                
                if stream_id not in stream_launch_info:
                    stream_launch_info[stream_id] = list()
                if stream_id not in stream_task_list:
                    stream_task_list[stream_id] = list()
                
                # append task info for event
                stream_task_list[stream_id].append(("event_info", event, arr[2], process_count))

                if is_record:
                    if event in event_info:
                        print(f"ERROR: {process_count} event {event} duplicated!", flush=True)
                        return
                    event_info[event] = (stream_id, len(stream_task_list[stream_id]))
                    # update record stream
                    stream_launch_info[stream_id].append(event)
                    # print(stream_launch_info[stream_id])
                else:
                    # process wait event
                    if event not in event_info:
                        print(f"ERROR: {process_count} event {event} not record first!", flush=True)
                        return
                    # udpate wait stream
                    # todo : may be need check cross stream event
                    # update sync info
                    record_stream_id, record_task_index = event_info[event]
                    if record_task_index > wait_info[int(record_stream_id)][int(stream_id)]:
                        wait_info[int(record_stream_id)][int(stream_id)] = record_task_index
                        # update hb info
                        for i in range(10):
                            if i == int(record_stream_id):
                                continue
                            wait_task_index = wait_info[i][int(record_stream_id)]
                            if wait_task_index > wait_info[i][int(stream_id)]:
                                print("update hp for", i, int(stream_id), wait_task_index)
                                wait_info[i][int(stream_id)] = wait_task_index
                                                
                    if stream_id == "0":
                        # print("process event info", line)
                        # find record stream id and process launch infos
                        remain = FindAndSlice(stream_launch_info[record_stream_id], event)
                        stream_launch_info[record_stream_id] = remain
                    del event_info[event]
            elif "lanunch_info" in line:

                def ExtractAddrAndSize(str):
                    addrs = dict()
                    if len(str) == 0:
                        return addrs
                    arr_str = str.split("|")
                    for arr in arr_str:
                        if len(arr) == 0:
                            continue
                        temp = arr.split("#")
                        addr = temp[0]
                        size = int(temp[1])
                        addrs[addr] = size
                    return addrs

                kernel_name = arr[1]
                stream_id = arr[2]
                # print(arr, flush=True)
                input_addrs = ExtractAddrAndSize(arr[3])
                output_addrs = ExtractAddrAndSize(arr[4])
                workspace_addrs = ExtractAddrAndSize(arr[5])
                # here input is reading addrs and output, workspace are writing addrs
                cur_reading_addrs = {**input_addrs}
                cur_writing_addrs = {**output_addrs, **workspace_addrs}
                launch_addrs = {
                    "reading_addrs": cur_reading_addrs,
                    "writing_addrs": cur_writing_addrs,
                }
                
                if stream_id not in stream_launch_info:
                    stream_launch_info[stream_id] = list()
                if stream_id not in stream_task_list:
                    stream_task_list[stream_id] = list()
                    
                # append task info for launch
                stream_task_list[stream_id].append(("launch_info", kernel_name, launch_addrs, process_count))

                # check launch addrs is valid
                for addr, size in cur_reading_addrs.items():
                    if addr == "0":
                        continue
                    if addr not in memory_info:
                        print(f"WARNING: {process_count} {kernel_name} addr {addr} not exist!", flush=True)
                        if not _CheckAddrValidInMemory(addr, size, memory_info):
                            print(f"ERROR: {process_count} {kernel_name} addr {addr} not valid!", flush=True)
                            return
                    if addr in memory_info and size != memory_info[addr]:
                        if memory_info[addr] - size > 512:
                            print(f"ERROR: {process_count} {kernel_name} addr {addr} waste more than 512!", flush=True)
                        if size > memory_info[addr]:
                            print(f"ERROR: {process_count} {kernel_name} addr {addr} overlap, {size} > {memory_info[addr]}!", flush=True)
                            return
                for addr, size in cur_writing_addrs.items():
                    if addr == "0":
                        continue
                    if addr not in memory_info:
                        print(f"ERROR: {process_count} {kernel_name} addr {addr} not exist!", flush=True)
                        if not _CheckAddrValidInMemory(addr, size, memory_info):
                            print(f"ERROR: {process_count} {kernel_name} addr {addr} not valid!", flush=True)
                            return
                    if addr in memory_info and size != memory_info[addr]:
                        if memory_info[addr] - size > 512:
                            print(f"ERROR: {process_count} {kernel_name} addr {addr} waste more than 512!", flush=True)
                        if size > memory_info[addr]:
                            print(f"ERROR: {process_count} {kernel_name} addr {addr} overlap, {size} > {memory_info[addr]}!", flush=True)
                            return

                stream_launch_info[stream_id].append(launch_addrs)
                
                def _CheckAddrOverlap(left, left_size, right, right_size):
                    left_start = int(left, 16)
                    right_start = int(right, 16)
                    left_end = left_start + left_size
                    right_end = right_start + right_size
                    return left_start < right_end and right_start < left_end
                def _CheckOverlap(addrs, candidate, candidate_size):
                    # print("CheckOverlap", addrs, candidate, candidate_size, flush=True)
                    for addr, size in addrs.items():
                        if _CheckAddrOverlap( addr, size, candidate, candidate_size):
                            print(f"ERROR: {process_count} addr {addr} and {candidate} overlapped!", flush=True)
                            return True
                    return False
                def CheckAddrsOverlap(left_addrs, right_addrs):
                    for addr, size in left_addrs.items():
                        if _CheckOverlap(right_addrs, addr, size):
                            print(f"ERROR: {process_count} addr {addr} and {kernel_name} overlapped!", flush=True)
                            return True
                    return False
                                  
                # wait info check
                for stream, task_list in stream_task_list.items():
                    # print("check on stream", stream, flush=True)
                    if stream == stream_id:
                        continue
                    # not support cross stream happens-before ana, this may cause false positives,
                    # but implements is not supportted, so just ignore it
                    safe_index = wait_info[int(stream)][int(stream_id)]
                    candidate_tasks = task_list[safe_index:]
                    # do check launch info and candidate tasks
                    for candidate_task in candidate_tasks:
                        task_name, kernel_name, launch_info, process_count = candidate_task
                        if task_name == "event_info":
                            continue
                        # process launch info
                        reading_addrs = launch_info["reading_addrs"]
                        writing_addrs = launch_info["writing_addrs"]
                        # check read/write conflicts
                        if CheckAddrsOverlap(cur_reading_addrs, writing_addrs):
                            print(f"ERROR: {process_count} {kernel_name} overlapped!", flush=True)
                            return
                        if CheckAddrsOverlap(cur_writing_addrs, writing_addrs):
                            print(f"ERROR: {process_count} {kernel_name} overlapped!", flush=True)
                            return
                        if CheckAddrsOverlap(cur_writing_addrs, reading_addrs):
                            print(f"ERROR: {process_count} {kernel_name} overlapped!", flush=True)
                            return
                    
                # old check on default stream
                if stream_id == "0":
                    # check addrs
                    for id, launch_infos in stream_launch_info.items():
                        # print("check on stream", id, flush=True)
                        if id == stream_id:
                            continue

                        for launch_info in launch_infos:
                            if not isinstance(launch_info, dict):
                                continue
                            # print("do check", launch_info)
                            # get reading addrs and writing addrs
                            reading_addrs = launch_info["reading_addrs"]
                            writing_addrs = launch_info["writing_addrs"]
                            # check read/write conflicts
                            if CheckAddrsOverlap(cur_reading_addrs, writing_addrs):
                                print(f"ERROR: {process_count} {kernel_name} overlapped!", flush=True)
                                return
                            if CheckAddrsOverlap(cur_writing_addrs, writing_addrs):
                                print(f"ERROR: {process_count} {kernel_name} overlapped!", flush=True)
                                return
                            if CheckAddrsOverlap(cur_writing_addrs, reading_addrs):
                                print(f"ERROR: {process_count} {kernel_name} overlapped!", flush=True)
                                return


if __name__ == "__main__":
    if len(sys.argv) >= 2:
        file_name = sys.argv[1]
        LoadFileAndCheck(file_name)
