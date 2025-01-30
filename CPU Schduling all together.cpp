#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <climits>
using namespace std;

struct Process {
    int id;
    int arrival_time;
    int burst_time;
    int priority;  // Added for priority scheduling
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int remaining_time;
};

void print_gantt_chart(const vector<pair<int, int>>& timeline) {
    cout << "\nGantt Chart:\n";
    
    // Print the upper border
    cout << " ";
    for (const auto& t : timeline) {
        cout << "--------";
    }
    cout << "\n";
    
    // Print process numbers
    cout << "|";
    for (const auto& t : timeline) {
        if (t.first == -1) {
            cout << "   X    |";  // idle
        } else {
            cout << "  P" << t.first << "   |";
        }
    }
    cout << "\n";
    
    // Print the lower border
    cout << " ";
    for (const auto& t : timeline) {
        cout << "--------";
    }
    cout << "\n";
    
    // Print the time line
    cout << "0";
    int current_time = 0;
    for (const auto& t : timeline) {
        current_time += t.second;
        cout << "       " << setw(1) << current_time;
    }
    cout << "\n";
}

// Function to implement FCFS scheduling
void fcfs(vector<Process>& processes) {
    sort(processes.begin(), processes.end(), 
        [](const Process& a, const Process& b) { return a.arrival_time < b.arrival_time; });
    
    vector<pair<int, int>> timeline;
    int current_time = 0;
    
    for(auto& p : processes) {
        if (current_time < p.arrival_time) {
            timeline.push_back({-1, p.arrival_time - current_time});
            current_time = p.arrival_time;
        }
        timeline.push_back({p.id, p.burst_time});
        p.completion_time = current_time + p.burst_time;
        p.turnaround_time = p.completion_time - p.arrival_time;
        p.waiting_time = p.turnaround_time - p.burst_time;
        current_time = p.completion_time;
    }
    
    print_gantt_chart(timeline);
}

// Function to implement non-preemptive SJF
void sjf_non_preemptive(vector<Process>& processes) {
    vector<Process> temp = processes;
    vector<Process> result;
    vector<pair<int, int>> timeline;
    int current_time = 0;
    
    while (!temp.empty()) {
        vector<Process> available;
        bool found = false;
        
        for (const auto& p : temp) {
            if (p.arrival_time <= current_time) {
                available.push_back(p);
                found = true;
            }
        }
        
        if (!found) {
            int next_arrival = INT_MAX;
            for (const auto& p : temp) {
                if (p.arrival_time < next_arrival) {
                    next_arrival = p.arrival_time;
                }
            }
            timeline.push_back({-1, next_arrival - current_time});
            current_time = next_arrival;
            continue;
        }
        
        // Sort by burst time, if equal then by arrival time
        sort(available.begin(), available.end(),
            [](const Process& a, const Process& b) {
                return (a.burst_time == b.burst_time) ? 
                       (a.arrival_time < b.arrival_time) : 
                       (a.burst_time < b.burst_time);
            });
        
        Process selected = available[0];
        selected.completion_time = current_time + selected.burst_time;
        selected.turnaround_time = selected.completion_time - selected.arrival_time;
        selected.waiting_time = selected.turnaround_time - selected.burst_time;
        
        timeline.push_back({selected.id, selected.burst_time});
        
        result.push_back(selected);
        current_time = selected.completion_time;
        
        temp.erase(remove_if(temp.begin(), temp.end(),
            [selected](const Process& p) { return p.id == selected.id; }), temp.end());
    }
    
    processes = result;
    print_gantt_chart(timeline);
}

// Function to implement preemptive SJF (SRTF)
void sjf_preemptive(vector<Process>& processes) {
    vector<Process> temp = processes;
    vector<pair<int, int>> timeline;
    int current_time = 0;
    int completed = 0;
    int prev_process = -1;
    int duration = 0;
    
    for(auto& p : temp) {
        p.remaining_time = p.burst_time;
    }
    
    while (completed != processes.size()) {
        int shortest_burst = INT_MAX;
        int shortest_index = -1;
        
        for (size_t i = 0; i < processes.size(); i++) {
            if (temp[i].arrival_time <= current_time && 
                temp[i].remaining_time < shortest_burst && 
                temp[i].remaining_time > 0) {
                shortest_burst = temp[i].remaining_time;
                shortest_index = i;
            }
        }
        
        if (shortest_index == -1) {
            if (prev_process != -1) {
                timeline.push_back({prev_process, duration});
                duration = 0;
            }
            prev_process = -1;
            
            // Find next arrival time
            int next_arrival = INT_MAX;
            for (const auto& p : temp) {
                if (p.remaining_time > 0 && p.arrival_time < next_arrival) {
                    next_arrival = p.arrival_time;
                }
            }
            timeline.push_back({-1, next_arrival - current_time});
            current_time = next_arrival;
            continue;
        }
        
        if (prev_process != temp[shortest_index].id && prev_process != -1) {
            timeline.push_back({prev_process, duration});
            duration = 0;
        }
        
        prev_process = temp[shortest_index].id;
        duration++;
        
        temp[shortest_index].remaining_time--;
        current_time++;
        
        if (temp[shortest_index].remaining_time == 0) {
            completed++;
            processes[shortest_index].completion_time = current_time;
            processes[shortest_index].turnaround_time = 
                processes[shortest_index].completion_time - processes[shortest_index].arrival_time;
            processes[shortest_index].waiting_time = 
                processes[shortest_index].turnaround_time - processes[shortest_index].burst_time;
            
            timeline.push_back({prev_process, duration});
            duration = 0;
            prev_process = -1;
        }
    }
    
    print_gantt_chart(timeline);
}

// Function to implement Round Robin scheduling
void round_robin(vector<Process>& processes, int quantum) {
    vector<Process> temp = processes;
    vector<pair<int, int>> timeline;
    queue<int> ready_queue;
    int current_time = 0;
    int completed = 0;
    vector<bool> in_queue(processes.size(), false);
    
    for(auto& p : temp) {
        p.remaining_time = p.burst_time;
    }
    
    // Add first process to queue
    for(size_t i = 0; i < processes.size(); i++) {
        if(temp[i].arrival_time == 0) {
            ready_queue.push(i);
            in_queue[i] = true;
        }
    }
    
    while (completed != processes.size()) {
        if (ready_queue.empty()) {
            // Find next arrival time
            int next_arrival = INT_MAX;
            for (size_t i = 0; i < processes.size(); i++) {
                if (!in_queue[i] && temp[i].remaining_time > 0 && 
                    temp[i].arrival_time < next_arrival) {
                    next_arrival = temp[i].arrival_time;
                }
            }
            timeline.push_back({-1, next_arrival - current_time});
            current_time = next_arrival;
            
            // Add newly arrived processes to queue
            for(size_t i = 0; i < processes.size(); i++) {
                if (!in_queue[i] && temp[i].remaining_time > 0 && 
                    temp[i].arrival_time <= current_time) {
                    ready_queue.push(i);
                    in_queue[i] = true;
                }
            }
            continue;
        }
        
        int current_process = ready_queue.front();
        ready_queue.pop();
        in_queue[current_process] = false;
        
        int execution_time = min(quantum, temp[current_process].remaining_time);
        timeline.push_back({temp[current_process].id, execution_time});
        
        temp[current_process].remaining_time -= execution_time;
        current_time += execution_time;
        
        // Add newly arrived processes to queue
        for(size_t i = 0; i < processes.size(); i++) {
            if (!in_queue[i] && temp[i].remaining_time > 0 && 
                temp[i].arrival_time <= current_time) {
                ready_queue.push(i);
                in_queue[i] = true;
            }
        }
        
        if (temp[current_process].remaining_time > 0) {
            ready_queue.push(current_process);
            in_queue[current_process] = true;
        } else {
            completed++;
            processes[current_process].completion_time = current_time;
            processes[current_process].turnaround_time = 
                processes[current_process].completion_time - processes[current_process].arrival_time;
            processes[current_process].waiting_time = 
                processes[current_process].turnaround_time - processes[current_process].burst_time;
        }
    }
    
    print_gantt_chart(timeline);
}

// Function to implement Priority scheduling (non-preemptive)
void priority_scheduling(vector<Process>& processes) {
    vector<Process> temp = processes;
    vector<Process> result;
    vector<pair<int, int>> timeline;
    int current_time = 0;
    
    while (!temp.empty()) {
        vector<Process> available;
        bool found = false;
        
        for (const auto& p : temp) {
            if (p.arrival_time <= current_time) {
                available.push_back(p);
                found = true;
            }
        }
        
        if (!found) {
            int next_arrival = INT_MAX;
            for (const auto& p : temp) {
                if (p.arrival_time < next_arrival) {
                    next_arrival = p.arrival_time;
                }
            }
            timeline.push_back({-1, next_arrival - current_time});
            current_time = next_arrival;
            continue;
        }
        
        // Sort by priority (lower number = higher priority)
        sort(available.begin(), available.end(),
            [](const Process& a, const Process& b) {
                return (a.priority == b.priority) ? 
                       (a.arrival_time < b.arrival_time) : 
                       (a.priority < b.priority);
            });
        
        Process selected = available[0];
        selected.completion_time = current_time + selected.burst_time;
        selected.turnaround_time = selected.completion_time - selected.arrival_time;
        selected.waiting_time = selected.turnaround_time - selected.burst_time;
        
        timeline.push_back({selected.id, selected.burst_time});
        
        result.push_back(selected);
        current_time = selected.completion_time;
        
        temp.erase(remove_if(temp.begin(), temp.end(),
            [selected](const Process& p) { return p.id == selected.id; }), temp.end());
    }
    
    processes = result;
    print_gantt_chart(timeline);
}

void print_results(const vector<Process>& processes) {
    cout << "\nProcess\tAT\tBT\tPR\tCT\tWT\tTT\n";
    cout << "------------------------------------------------\n";
    
    double avg_waiting_time = 0;
    double avg_turnaround_time = 0;
    
    for (const auto& p : processes) {
        cout << "P" << p.id << "\t"
             << p.arrival_time << "\t"
             << p.burst_time << "\t"
             << p.priority << "\t"
             << p.completion_time << "\t"
             << p.waiting_time << "\t"
             << p.turnaround_time << "\n";
             
        avg_waiting_time += p.waiting_time;
        avg_turnaround_time += p.turnaround_time;
    }
    
    avg_waiting_time /= processes.size();
    avg_turnaround_time /= processes.size();
    
    cout << "\nAvg Waiting Time: " << fixed << setprecision(2) << avg_waiting_time;
    cout << "\nAvg Turnaround Time: " << avg_turnaround_time << "\n";
}

int main() {
    cout << "Scheduling Algorithm:\n";
    cout << "1. FCFS\n";
    cout << "2. SJF (non-preemptive)\n";
    cout << "3. SJF (preemptive)\n";
    cout << "4. Round Robin\n";
    cout << "5. Priority (non-preemptive)\n";
    cout << "Enter an option: ";
    
    int choice;
    cin >> choice;
    
    cout << "\nEnter number of processes: ";
    int n;
    cin >> n;
    
    vector<Process> processes(n);
    
    for (int i = 0; i < n; i++) {
        processes[i].id = i + 1;
        cout << "\nProcess " << i + 1 << ":\n";
        cout << "Arrival Time: ";
        cin >> processes[i].arrival_time;
        cout << "Burst Time: ";
        cin >> processes[i].burst_time;
        if (choice == 5) {
            cout << "Priority (lower number = higher priority): ";
            cin >> processes[i].priority;
        } else {
            processes[i].priority = 0;
        }
        processes[i].remaining_time = processes[i].burst_time;
    }
    
    int quantum;
    if (choice == 4) {
        cout << "\nEnter time quantum: ";
        cin >> quantum;
    }
    
    switch(choice) {
            case 1:
                cout << "\nFCFS Scheduling\n";
                fcfs(processes);
                break;
                  case 2:
            cout << "\nSJF (non-preemptive)\n";
            sjf_non_preemptive(processes);
            break;
        case 3:
            cout << "\nSJF (preemptive)\n";
            sjf_preemptive(processes);
            break;
        case 4:
            cout << "\nRound Robin\n";
            round_robin(processes, quantum);
            break;
        case 5:
            cout << "\nPriority (non-preemptive)\n";
            priority_scheduling(processes);
            break;
        default:
            cout << "Invalid option!\n";
            return 1;
    }
    
    print_results(processes);
    
    return 0;
}