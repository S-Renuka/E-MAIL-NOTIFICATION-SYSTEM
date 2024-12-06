#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Define the NotificationType enumeration
typedef enum {
    OFFER,
    REMINDER,
    ALERT
} NotificationType;

// Define the Notification structure
typedef struct Notification {
    NotificationType type;
    char subject[100];    // Used for Offer and Alert Notifications
    char body[500];
    char recipient[100];
    int year;
    int month;
    int day;
    time_t sendTime;      // Timestamp for when the notification should be sent
    struct Notification *next;
} Notification;

// Define the Queue structure
typedef struct Queue {
    Notification *front;
    Notification *rear;
} Queue;

// Function to create a new notification
Notification* createNotification(NotificationType type, char* subject, char* body, char* recipient, int year, int month, int day, time_t sendTime) {
    Notification *newNotification = (Notification *)malloc(sizeof(Notification));
    if (newNotification == NULL) {
        printf("Error: Memory allocation failed.\n");
        return NULL;
    }
    newNotification->type = type;
    strcpy(newNotification->subject, subject);
    strcpy(newNotification->body, body);
    strcpy(newNotification->recipient, recipient);
    newNotification->year = year;
    newNotification->month = month;
    newNotification->day = day;
    newNotification->sendTime = sendTime;
    newNotification->next = NULL;
    return newNotification;
}

// Function to initialize the queue
void initQueue(Queue *q) {
    q->front = q->rear = NULL;
}

// Function to enqueue a notification into the queue
void enqueue(Queue *q, Notification *newNotification) {
    if (q->rear == NULL) {
        q->front = q->rear = newNotification;
        return;
    }
    q->rear->next = newNotification;
    q->rear = newNotification;
}

// Function to dequeue a notification from the queue
Notification* dequeue(Queue *q) {
    if (q->front == NULL)
        return NULL;
    Notification *temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;
    return temp;
}

// Function to send an offer notification
void sendOffer(Notification *notification) {
    printf("\nSending Offer Reminder to: %s\n", notification->recipient);
    printf("Body: %s\n", notification->body);
}

// Function to send a reminder notification
void sendReminder(Notification *notification) {
    printf("\nSending Reminder to: %s\n", notification->recipient);
    printf("Body: %s\n", notification->body);
}

// Function to send an alert notification
void sendAlert(Notification *notification) {
    printf("Sending Alert Reminder to: %s\n", notification->recipient);
    printf("Body: %s\n", notification->body);
}

// Function to send a notification based on its type
void sendNotificationType(Notification *notification) {
    switch (notification->type) {
        case OFFER:
            sendOffer(notification);
            break;
        case REMINDER:
            sendReminder(notification);
            break;
        case ALERT:
            sendAlert(notification);
            break;
        default:
            printf("Unknown notification type.\n");
            break;
    }
}

// Function to read notification details and determine the type automatically
Notification* readNotificationDetails(int notificationIndex) {
    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);
    int year = localTime->tm_year + 1900;  // Year since 1900
    int month = localTime->tm_mon + 1;     // Month, 0 - jan
    int day = localTime->tm_mday;          // Day of the month

    // Define the recipient within the program
    char recipient[100] = "receiver@gmail.com"; // Placeholder phone number for SMS

    // Define multiple predefined notifications
    struct {
        NotificationType type;
        char subject[100];
        char body[500];
    } predefinedNotifications[] = {
        { OFFER, "", "Exclusive Offer! Get 20% off on your next purchase. Visit our store now!" },
        { REMINDER, "", "Reminder: Your appointment is scheduled for tomorrow at 10 AM. Please be on time." },
        { ALERT, "", "Alert: Suspicious login attempt detected. If this was not you, please reset your password immediately." }
    };

    // Automatically select the notification type based on the index
    int predefinedCount = sizeof(predefinedNotifications) / sizeof(predefinedNotifications[0]);
    int selectedNotificationIndex = notificationIndex % predefinedCount;

    // Get the details of the selected notification
    NotificationType notificationType = predefinedNotifications[selectedNotificationIndex].type;
    char subject[100];
    char body[500];
    strcpy(subject, predefinedNotifications[selectedNotificationIndex].subject);
    strcpy(body, predefinedNotifications[selectedNotificationIndex].body);

    // Set the send time to 10 seconds from the current time
    time_t sendTime = currentTime + 10;

    return createNotification(notificationType, subject, body, recipient, year, month, day, sendTime);
}

// Main function
int main() {
    Queue notificationQueue;
    initQueue(&notificationQueue);

    int numNotifications;
    printf("Enter number of notifications to schedule: ");
    scanf("%d", &numNotifications);
    getchar(); // Consume the newline character left by scanf

    // Read notification details and enqueue them
    for (int i = 0; i < numNotifications; i++) {
        printf("\nScheduling reminder %d...\n", i + 1);
        Notification *notification = readNotificationDetails(i);
        if (notification != NULL) {
            enqueue(&notificationQueue, notification);
        }
    }

    // Process the queue to send notifications at the scheduled times
    while (notificationQueue.front != NULL) {
        time_t currentTime = time(NULL);
        Notification *notificationToSend = notificationQueue.front;
        if (currentTime >= notificationToSend->sendTime) {
            notificationToSend = dequeue(&notificationQueue);
            sendNotificationType(notificationToSend);
            free(notificationToSend); // Free the memory after sending the notification
        } else {
            // Sleep for a short period to avoid busy waiting
            sleep(1);
        }
    }

    return 0;
}