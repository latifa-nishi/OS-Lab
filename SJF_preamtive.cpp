#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <climits>
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
    int current_time = 0;
    int completed = 0;
    int prev_process = -1;
    int duration = 0;
    
    while (completed != n) {
        int shortest_burst = INT_MAX;
        int shortest_index = -1;
        
        for (int i = 0; i < n; i++) {
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
    
    cout << "\nSJF Preemptive Scheduling Results:\n";
    print_gantt_chart(timeline);
    print_results(processes);
    
    return 0;
}