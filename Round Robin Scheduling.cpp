#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
using namespace std;

struct Process {
    int id;
    int arrival_time;
    int burst_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int remaining_time;
};

void print_gantt_chart(const vector<pair<int, int>>& timeline) {
    cout << "\nGantt Chart:\n";
    cout << " ";
    for (const auto& t : timeline) {
        cout << "--------";
    }
    cout << "\n|";
    
    for (const auto& t : timeline) {
        if (t.first == -1) {
            cout << "   X    |";
        } else {
            cout << "  P" << t.first << "   |";
        }
    }
    cout << "\n ";
    
    for (const auto& t : timeline) {
        cout << "--------";
    }
    cout << "\n0";
    
    int current_time = 0;
    for (const auto& t : timeline) {
        current_time += t.second;
        cout << "       " << setw(1) << current_time;
    }
    cout << "\n";
}

void print_results(const vector<Process>& processes) {
    cout << "\nProcess\tAT\tBT\tCT\tWT\tTT\n";
    cout << "----------------------------------------\n";
    
    double avg_waiting_time = 0;
    double avg_turnaround_time = 0;
    
    for (const auto& p : processes) {
        cout << "P" << p.id << "\t"
             << p.arrival_time << "\t"
             << p.burst_time << "\t"
             << p.completion_time << "\t"
             << p.waiting_time << "\t"
             << p.turnaround_time << "\n";
             
        avg_waiting_time += p.waiting_time;
        avg_turnaround_time += p.turnaround_time;
    }
    
    avg_waiting_time /= processes.size();
    avg_turnaround_time /= processes.size();
    
    cout << "\nAverage Waiting Time: " << fixed << setprecision(2) << avg_waiting_time;
    cout << "\nAverage Turnaround Time: " << avg_turnaround_time << "\n";
}

int main() {
    cout << "Enter number of processes: ";
    int n;
    cin >> n;
    
    cout << "Enter time quantum: ";
    int quantum;
    cin >> quantum;
    
    vector<Process> processes(n);
    vector<Process> temp(n);
    
    for (int i = 0; i < n; i++) {
        processes[i].id = i + 1;
        cout << "\nProcess " << i + 1 << ":\n";
        cout << "Arrival Time: ";
        cin >> processes[i].arrival_time;
        cout << "Burst Time: ";
        cin >> processes[i].burst_time;
        processes[i].remaining_time = processes[i].burst_time;
        temp[i] = processes[i];
    }
    
    vector<pair<int, int>> timeline;
    queue<int> ready_queue;
    int current_time = 0;
    int completed = 0;
    vector<bool> in_queue(n, false);
    
    // Add first process to queue
    for(int i = 0; i < n; i++) {
        if(temp[i].arrival_time == 0) {
            ready_queue.push(i);
            in_queue[i] = true;
        }
    }
    
    while (completed != n) {
        if (ready_queue.empty()) {
            int next_arrival = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (!in_queue[i] && temp[i].remaining_time > 0 && 
                    temp[i].arrival_time < next_arrival) {
                    next_arrival = temp[i].arrival_time;
                }
            }
            timeline.push_back({-1, next_arrival - current_time});
            current_time = next_arrival;
            
            for(int i = 0; i < n; i++) {
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
        
        for(int i = 0; i < n; i++) {
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
    
    cout << "\nRound Robin Scheduling Results (Time Quantum = " << quantum << "):\n";
    print_gantt_chart(timeline);
    print_results(processes);
    
    return 0;
}