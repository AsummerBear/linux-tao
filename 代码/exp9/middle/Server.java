package com.exp9;

import java.io.*;
import java.net.BindException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.StringTokenizer;

public class Server {

    private List<User> userList = new ArrayList<>();
    private ServerSocket serverSocket;
    private ServerThread serverThread;
    private ArrayList<ClientThread> clients;
    private Socket chatSocket;
    private PrintStream chatWriter;
    private BufferedReader chatReader;
    private ChatMessageThread chatMessageThread;
    private boolean isStart = false;
    private boolean robot = false;

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        System.out.print("输入端口：");
        int port = sc.nextInt();
        System.out.print("输入最大连接数：");
        int max = sc.nextInt();
        System.out.print("输入AI服务IP：");
        String host = sc.next();
        System.out.print("输入AI服务端口：");
        int chatPort = sc.nextInt();
        try {
            new Server().serverStart(max,port,host,chatPort);
        } catch (BindException e) {
            e.printStackTrace();
        }
    }

    // 执行消息发送
    public void send(String message) {
        if (!isStart) {
            System.out.println("服务器还未启动,不能发送消息！");
            return;
        }
        if (clients.size() == 0) {
            System.out.println("没有用户在线,不能发送消息！");
            return;
        }
        if (message == null || message.equals("")) {
            System.out.println("消息并不能为空！");
            return;
        }
        sendServerMessage(message);// 群发服务器消息
    }

    // 启动服务器
    public void serverStart(int max, int port,String host,int chatPort) throws BindException {
        try {
            clients = new ArrayList<>();
            serverSocket = new ServerSocket(port);
            serverThread = new ServerThread(serverSocket, max);
            serverThread.start();
            isStart = true;
            chatSocket = new Socket(host, chatPort);
            chatWriter = new PrintStream(chatSocket.getOutputStream());
            chatReader = new BufferedReader(new InputStreamReader(chatSocket.getInputStream(), "utf-8"));
            chatMessageThread = new ChatMessageThread(chatReader);
            chatMessageThread.start();
        } catch (BindException e) {
            isStart = false;
            throw new BindException("端口号已被占用，请换一个！");
        } catch (Exception e1) {
            e1.printStackTrace();
            isStart = false;
            throw new BindException("启动服务器异常！");
        }
        new OptionThread().start();
    }

    // 关闭服务器
    @SuppressWarnings("deprecation")
    public void closeServer() {
        try {
            if (serverThread != null)
                serverThread.stop();// 停止服务器线程

            for (int i = clients.size() - 1; i >= 0; i--) {
                // 给所有在线用户发送关闭命令
                clients.get(i).getWriter().println("CLOSE");
                clients.get(i).getWriter().flush();
                // 释放资源
                clients.get(i).stop();// 停止此条为客户端服务的线程
                clients.get(i).reader.close();
                clients.get(i).writer.close();
                clients.get(i).socket.close();
                clients.remove(i);
            }
            if (serverSocket != null) {
                chatSocket.close();
                serverSocket.close();// 关闭服务器端连接
            }
            userList.clear();// 清空用户列表
            isStart = false;
        } catch (IOException e) {
            e.printStackTrace();
            isStart = true;
        }
    }

    // 群发服务器消息
    public void sendServerMessage(String message) {
        for (int i = clients.size() - 1; i >= 0; i--) {
            clients.get(i).getWriter().println("服务器：" + message);
            clients.get(i).getWriter().flush();
        }
    }

    public void controlRobot(boolean state){
        this.robot = state;
    }

    public void showUserList(){
        for (User user:userList) {
            System.out.println(user.getName()+"("+user.getIp()+")");
        }
    }

    // 转发消息
    public void dispatcherMessage(String message) {
        StringTokenizer stringTokenizer = new StringTokenizer(message, "@");
        String source = stringTokenizer.nextToken();
        String owner = stringTokenizer.nextToken();
        String content = stringTokenizer.nextToken();
        message = source + "说：" + content;
        //System.out.println(message);
        if (owner.equals("ALL")) {// 群发
            for (int i = clients.size() - 1; i >= 0; i--) {
                clients.get(i).getWriter().println(message);
                clients.get(i).getWriter().flush();
            }
        }
    }

    // 服务器线程
    class ServerThread extends Thread {
        private ServerSocket serverSocket;
        private int max;// 人数上限

        // 服务器线程的构造方法
        public ServerThread(ServerSocket serverSocket, int max) {
            this.serverSocket = serverSocket;
            this.max = max;
        }

        public void run() {
            while (true) {// 不停的等待客户端的链接
                try {
                    Socket socket = serverSocket.accept();
                    if (clients.size() == max) {// 如果已达人数上限
                        BufferedReader r = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                        PrintWriter w = new PrintWriter(socket.getOutputStream());
                        // 接收客户端的基本用户信息
                        String inf = r.readLine();
                        StringTokenizer st = new StringTokenizer(inf, "@");
                        User user = new User(st.nextToken(), st.nextToken());
                        // 反馈连接成功信息
                        w.println("MAX@服务器：对不起，" + user.getName() + user.getIp() + "，服务器在线人数已达上限，请稍后尝试连接！");
                        w.flush();
                        // 释放资源
                        r.close();
                        w.close();
                        socket.close();
                        continue;
                    }
                    ClientThread client = new ClientThread(socket);
                    client.start();// 开启对此客户端服务的线程
                    clients.add(client);
                    userList.add(client.getUser());// 更新在线列表
                    System.out.println(client.getUser().getName() + client.getUser().getIp() + "上线!\r\n");
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    // 为一个客户端服务的线程
    class ClientThread extends Thread {
        private Socket socket;
        private BufferedReader reader;
        private PrintWriter writer;
        private User user;

        public BufferedReader getReader() {
            return reader;
        }

        public PrintWriter getWriter() {
            return writer;
        }

        public User getUser() {
            return user;
        }

        // 客户端线程的构造方法
        public ClientThread(Socket socket) {
            try {
                this.socket = socket;
                reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                writer = new PrintWriter(socket.getOutputStream());
                // 接收客户端的基本用户信息
                String inf = reader.readLine();
                StringTokenizer st = new StringTokenizer(inf, "@");
                user = new User(st.nextToken(), st.nextToken());
                // 反馈连接成功信息
                writer.println(user.getName() + user.getIp() + "与服务器连接成功!");
                writer.flush();
                // 反馈当前在线用户信息
                if (clients.size() > 0) {
                    String temp = "";
                    for (int i = clients.size() - 1; i >= 0; i--) {
                        temp += (clients.get(i).getUser().getName() + "/" + clients.get(i).getUser().getIp()) + "@";
                    }
                    writer.println("USERLIST@" + clients.size() + "@" + temp);
                    writer.flush();
                }
                // 向所有在线用户发送该用户上线命令
                for (int i = clients.size() - 1; i >= 0; i--) {
                    clients.get(i).getWriter().println("ADD@" + user.getName() + user.getIp());
                    clients.get(i).getWriter().flush();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        @SuppressWarnings("deprecation")
        public void run() {// 不断接收客户端的消息，进行处理。
            String message = null;
            String res = null;
            while (true) {
                try {
                    message = reader.readLine();// 接收客户端消息
                    if(robot){
                        StringTokenizer stringTokenizer = new StringTokenizer(message, "@");
                        String source = stringTokenizer.nextToken();
                        String owner = stringTokenizer.nextToken();
                        String content = stringTokenizer.nextToken();
                        chatWriter.println(content+"over");  //将客户端消息发送给机器人
                    }
                    if (message.equals("CLOSE"))// 下线命令
                    {
                        System.out.println(this.getUser().getName() + this.getUser().getIp() + "下线!\r\n");
                        // 断开连接释放资源
                        reader.close();
                        writer.close();
                        socket.close();

                        // 向所有在线用户发送该用户的下线命令
                        for (int i = clients.size() - 1; i >= 0; i--) {
                            clients.get(i).getWriter().println("DELETE@" + user.getName());
                            clients.get(i).getWriter().flush();
                        }
                        userList.remove(this.getUser());
                        // 删除此条客户端服务线程
                        for (int i = clients.size() - 1; i >= 0; i--) {
                            if (clients.get(i).getUser() == user) {
                                ClientThread temp = clients.get(i);
                                clients.remove(i);// 删除此用户的服务线程
                                temp.stop();// 停止这条服务线程
                                return;
                            }
                        }
                    } else {
                        dispatcherMessage(message);// 转发消息
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    //接收AI服务器消息的线程
    class ChatMessageThread extends Thread{
        private BufferedReader reader;

        public ChatMessageThread(BufferedReader reader){
            this.reader = reader;
        }

        public void run() {
            String message = "";
            while (true) {
                try {
                    message = reader.readLine();
                    dispatcherMessage(message);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }

    }

    //操作线程
    class OptionThread extends Thread{
        @Override
        public void run() {
            String msg = "";
            Scanner sc = new Scanner(System.in);
            while (isStart){
                System.out.println("选择操作：1:当前用户列表\t2:群发消息\t3:聊天机器人\t0:关闭服务");
                int op = sc.nextInt();
                switch (op){
                    case 0:
                        closeServer();break;
                    case 1:
                        showUserList();break;
                    case 2:
                        System.out.print("输入群发消息：");
                        msg = sc.next();
                        send(msg);break;
                    case 3:
                        System.out.print("选择是否开启[y/n]");
                        msg = sc.next();
                        if(msg.equals("y")||msg.equals("true")){
                            System.out.println("聊天机器人已开启");
                            controlRobot(true);
                        }else{
                            System.out.println("聊天机器人已关闭");
                            controlRobot(false);
                        }
                    default:break;
                }
            }
        }
    }

}
