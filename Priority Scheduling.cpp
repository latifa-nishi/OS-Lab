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
    int priority;  // Lower number = higher priority
    int completion_time;
    int waiting_time;
    int turnaround_time;
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
    
    cout << "\nAverage Waiting Time: " << fixed << setprecision(2) << avg_waiting_time;
    cout << "\nAverage Turnaround Time: " << avg_turnaround_time << "\n";
}

int main() {
    cout << "Enter number of processes: ";
    int n;
    cin >> n;
    
    vector<Process> processes(n);
    vector<Process> temp(n);
    vector<Process> result;
    
    for (int i = 0; i < n; i++) {
        processes[i].id = i + 1;
        cout << "\nProcess " << i + 1 << ":\n";
        cout << "Arrival Time: ";
        cin >> processes[i].arrival_time;
        cout << "Burst Time: ";
        cin >> processes[i].burst_time;
        cout << "Priority (lower number = higher priority): ";
        cin >> processes[i].priority;
        temp[i] = processes[i];
    }
    
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
    
    cout << "\nPriority Scheduling Results:\n";
    print_gantt_chart(timeline);
    print_results(processes);
    
    return 0;
}